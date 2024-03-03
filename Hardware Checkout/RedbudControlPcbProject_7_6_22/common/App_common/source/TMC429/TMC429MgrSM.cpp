/**
 * @file TMC429MgrSM.cpp
 *
 * @author awong
 *
 * QP Interface:
 * Subscribe:
 * - TMC429_TRANSACTION_COMPLETE_SIG:
 *   Signal passed to SPIMgr AO.  This signal is published by the SPIMgr when a transaction has been
 *   completed.
 * - TMC429_CONFIG_MOTOR_SIG:
 *   Event with config parameters for a stepper motor.
 * - TMC429_SET_MOTOR_POSITION_SIG:
 *   Event with target position for a stepper motor.
 * - TMC429_SET_MOTOR_VELOCITY_SIG:
 *   Event with target velocity for a stepper motor.
 * - TMC429_RAW_COMMAND_SIG:
 *   Event with raw 4-byte SPI transaction to be sent to the TMC429.
 * - TMC429_HOMING_COMMAND_SIG:
 *   Event that begins a homing sequence for selected stepper motors.
 * - TMC429_QUERY_MOTOR_FEEDBACK_SIG:
 *   TODO:
 * - TMC429_SET_MODE_SIG:
 *   Event that sets the mode (MotionModes) for the selected stepper motor.
 * - TMC429_SET_CURRENT_SCALING_SIG:
 *   Event that contains current scaling for a stepper motor.
 *
 * Publish:
 * - SPIn_TRANSACTION_SIG:
 *   Event with a SPI transaction for the TMC429.
 * - TMC429_HOMING_COMMAND_COMPLETE_SIG:
 *   Event indicates either homing has completed or has an error.
 * - TMC429_MOTOR_FEEDBACK_SIG:
 *   TODO:
 *
 * State Machine:
 * - running
 *   Manages timer that queries for actual position, velocity, and interrupt flags.
 *   - idle
 *     Ready to receive and process a new transaction.
 *   - busy
 *     SPI transaction(s) in progress, so defer any new events that come in until back in idle.
 *     - startup
 *       Manages startup sequence when TMC429 is initialized.
 *       - check_id
 *         Request ID from the TMC429 and validates that it is correct.
 *       - send_init_config
 *         Sends initialization configuration with number of motors, etc. to the TMC429.
 *       - send_driver_chain_table
 *         Note: This state is skipped.
 *         Sends driver chain table to the TMC429.
 *     - homing
 *       Manages homing sequence when homing is requested for selected stepper motors.
 *       - send_homing_command
 *         Sends homing sequence to each selected stepper motor which includes:
 *         1) Setting velocity to 0.
 *         2) Setting to velocity mode.
 *         3) Setting velocity to -50.
 *       - homing_switch_wait
 *         Periodically sends request to obtain reference switch flags from the TMC429.  Transitions
 *         to send_homing_zero state when all homed stepper motors reach left reference switch.
 *       - send_homing_zero
 *         1) Sets velocity back to 0.
 *         Note: Expectation is that user will set the mode back to desired mode after homing is complete.
 *     - command_wait
 *       Waits for single SPI transaction to be completed.
 *     - config_wait
 *       Waits for all config SPI transactions to be completed.
 *     - query_motor_wait
 *       Sends queries for actual position, velocity, and interrupt flags, and then waits for the
 *       response.
 * - fault
 *   SPI error has occurred.
 */

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qspy_bsp.h"

#include "TMC429Mgr.h"
#include "TMC429Protocol.h"

#include <array>

Q_DEFINE_THIS_MODULE("TMC429MgrSM")

namespace COMMON {
namespace TMC429 {
using namespace COMMON::SPI;

TMC429Mgr::TMC429Mgr(const GpioPin chip_select_pin,
					 const tmc429_driver_chain_table &driver_chain_table,
					 const LSMD_NumberMotorDrivers num_motors,
					 const IfConfiguration config)
: QActive(Q_STATE_CAST(&TMC429Mgr::initial)),
  HomingQueryTimer(this, HOMING_PERIODIC_QUERY_SIG, 0U),
  chip_select_pin(chip_select_pin),
  if_config(config),
  status{false, false, {false, false, false}, {false, false, false}},
  driver_chain_state(driver_chain_table),
  pmul_pdiv(),
  num_motor_drivers(num_motors)
{
    DeferredEventQueue.init(DeferredEventQSto, Q_DIM(DeferredEventQSto));
}

Q_STATE_DEF(TMC429Mgr, initial) {
    (void)e;

    QS_FUN_DICTIONARY(&running);
    QS_FUN_DICTIONARY(&idle);
    QS_FUN_DICTIONARY(&busy);
    QS_FUN_DICTIONARY(&startup);
    QS_FUN_DICTIONARY(&check_id);
    QS_FUN_DICTIONARY(&send_init_config);
    QS_FUN_DICTIONARY(&send_driver_chain_table);
    QS_FUN_DICTIONARY(&homing);
    QS_FUN_DICTIONARY(&send_homing_command);
    QS_FUN_DICTIONARY(&homing_switch_wait);
    QS_FUN_DICTIONARY(&send_homing_zero);
    QS_FUN_DICTIONARY(&command_wait);
    QS_FUN_DICTIONARY(&config_wait);
    QS_FUN_DICTIONARY(&query_motor_feedback);
    QS_FUN_DICTIONARY(&fault);

    subscribe(TMC429_TRANSACTION_COMPLETE_SIG);
    subscribe(TMC429_CONFIG_MOTOR_SIG);
    subscribe(TMC429_SET_MOTOR_POSITION_SIG);
    subscribe(TMC429_SET_MOTOR_VELOCITY_SIG);
    subscribe(TMC429_RAW_COMMAND_SIG);
    subscribe(TMC429_HOMING_COMMAND_SIG);
    subscribe(TMC429_SET_MODE_SIG);
    subscribe(TMC429_SET_CURRENT_SCALING_SIG);
    subscribe(TMC429_QUERY_MOTOR_FEEDBACK_SIG);

    return tran(&startup);
}

Q_STATE_DEF(TMC429Mgr, running) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    default:
        status_ = super(&top);
        break;
    }

    return status_;
}

// running::idle
Q_STATE_DEF(TMC429Mgr, idle) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
        recall(&DeferredEventQueue);
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_CONFIG_MOTOR_SIG:
        handleConfigMotorEvent(static_cast<const TMC429::ConfigMotorEvent *>(e));
        status_ = tran(&config_wait);
        break;

    case TMC429_SET_MOTOR_POSITION_SIG:
        PublishMotorPositionCommand(static_cast<const TMC429::SetMotorPositionEvent *>(e));
        status_ = tran(&command_wait);
        break;

    case TMC429_SET_MOTOR_VELOCITY_SIG:
        PublishMotorVelocityCommand(static_cast<const TMC429::SetMotorVelocityEvent *>(e));
        status_ = tran(&command_wait);
        break;

    case TMC429_RAW_COMMAND_SIG:
    	PublishMotorRawCommand(static_cast<const TMC429::RawCommandEvent *>(e));
    	status_ = tran(&command_wait);
    	break;

    case TMC429_HOMING_COMMAND_SIG:
    	HomingCommandAssert(static_cast<const TMC429::HomingCommandEvent *>(e));
    	homing_state.reset();
    	homing_state.set_motors_homed(static_cast<const TMC429::HomingCommandEvent *>(e)->homing_enabled);
    	homing_state.set_motors_homing_velocity(static_cast<const TMC429::HomingCommandEvent *>(e)->homing_velocity);
    	status_ = tran(&send_homing_command);
    	break;

    case TMC429_SET_MODE_SIG:
    	PublishMotorModeCommand(static_cast<const TMC429::SetModeEvent *>(e));
    	status_ = tran(&command_wait);
    	break;

    case TMC429_SET_CURRENT_SCALING_SIG:
    	PublishMotorCurrentScalingCommand(static_cast<const TMC429::SetCurrentScalingEvent *>(e));
    	status_ = tran(&command_wait);
    	break;

    case TMC429_QUERY_MOTOR_FEEDBACK_SIG:
    	query_state.reset();
    	query_state.set_motor_index(static_cast<StepperMotorIndex>(static_cast<const TMC429::QueryMotorFeedbackEvent *>(e)->motor_index));
    	status_ = tran(&query_motor_feedback);
    	break;

    default:
        status_ = super(&running);
        break;
    }

    return status_;
}

// running::busy
Q_STATE_DEF(TMC429Mgr, busy) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

	// If we get any events that require us to send a SPI transaction while we're busy, defer them
	// until we're not busy.
	case TMC429_CONFIG_MOTOR_SIG:
	case TMC429_SET_MOTOR_POSITION_SIG:
	case TMC429_SET_MOTOR_VELOCITY_SIG:
	case TMC429_RAW_COMMAND_SIG:
	case TMC429_HOMING_COMMAND_SIG:
	case TMC429_SET_MODE_SIG:
	case TMC429_SET_CURRENT_SCALING_SIG:
	case TMC429_QUERY_MOTOR_FEEDBACK_SIG:
		defer(&DeferredEventQueue, e);
		status_ = Q_RET_HANDLED;
		break;

    default:
        status_ = super(&running);
        break;
    }

    return status_;
}

// running::busy::startup
Q_STATE_DEF(TMC429Mgr, startup) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	PostStartOperation();
    	status_ = Q_RET_HANDLED;
    	break;

    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case START_OPERATION_SIG:
    	status_ = tran(&check_id);
    	break;

    default:
        status_ = super(&busy);
        break;
    }

    return status_;
}

// running::busy::startup::check_id
Q_STATE_DEF(TMC429Mgr, check_id) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG: {
        auto transaction = build_transaction::read_version();
        PublishSpiTransaction(transaction);
        status_ = Q_RET_HANDLED;
        break;
    }
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_read_version_correct(static_cast<const TransactionCompleteEvent *>(e))) {
            record_status(static_cast<const TransactionCompleteEvent *>(e));
#if CONFIG_ENABLE_TMC429_DRIVER_CHAIN_TABLE_INIT
            status_ = tran(&send_driver_chain_table);
#else
            status_ = tran(&send_init_config);
#endif
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&startup);
        break;
    }

    return status_;
}

// running::busy::startup::send_driver_chain_table
Q_STATE_DEF(TMC429Mgr, send_driver_chain_table) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	driver_chain_state.reset();
    	SendDriverChainTableTransaction();
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
        	driver_chain_state.mark_transaction_complete();
        	if (driver_chain_state.is_driver_chain_complete()) {
        		status_ = tran(&send_init_config);
        	}
        	else {
        		SendDriverChainTableTransaction();
        		status_ = Q_RET_HANDLED;
        	}
        }
        else {
        	status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&startup);
        break;
    }

    return status_;
}

// running::busy::startup::send_init_config
Q_STATE_DEF(TMC429Mgr, send_init_config) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	config_received_packets_count = 0;
    	SendInitConfig();
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            config_received_packets_count++;
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            // TODO: Make this not a hard-coded value.
            if (config_received_packets_count >= 8) {
                status_ = tran(&idle);
            }
            else {
                status_ = Q_RET_HANDLED;
            }
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&startup);
        break;
    }

    return status_;
}

// running::busy::homing
Q_STATE_DEF(TMC429Mgr, homing) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	status_ = Q_RET_HANDLED;
    	break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

	// TODO: Could monitor for homing cancel event.
	// TODO: Could monitor for homing timeout.

    default:
        status_ = super(&busy);
        break;
    }

    return status_;
}

// running::busy::homing::send_homing_command
Q_STATE_DEF(TMC429Mgr, send_homing_command) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	SendHomingCommands();
    	status_ = Q_RET_HANDLED;
    	break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            homing_state.collect_command_response();
            status_ = homing_state.is_command_complete() ? tran(&homing_switch_wait) : Q_RET_HANDLED;
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&homing);
        break;
    }

    return status_;
}

// TODO: Should probably add a timeout here or some way to cancel the homing sequence if it takes too
//       long.
// running::busy::homing::homing_switch_wait
Q_STATE_DEF(TMC429Mgr, homing_switch_wait) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	HomingQueryTimer.armX(HomingQueryTicks, HomingQueryTicks);
    	status_ = Q_RET_HANDLED;
    	break;
    case Q_EXIT_SIG:
    	HomingQueryTimer.disarm();
        status_ = Q_RET_HANDLED;
        break;

    case HOMING_PERIODIC_QUERY_SIG:
    	// Note: We're not actually inspecting anything about this read command, but we could
    	//       in the future if we wanted.
    	QueryReferenceSwitches();
    	status_ = Q_RET_HANDLED;
    	break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            status_ = homing_state.switches_hit(status) ? tran(&send_homing_zero) : Q_RET_HANDLED;
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&homing);
        break;
    }

    return status_;
}

// running::busy::homing::send_homing_zero
Q_STATE_DEF(TMC429Mgr, send_homing_zero) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	SendHomingZero();
    	status_ = Q_RET_HANDLED;
    	break;
    case Q_EXIT_SIG:
    	// TODO: For now always publish no error.
    	PublishHomingCommandComplete(TMC429NoError);
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            homing_state.collect_zero_response();
            status_ = homing_state.is_zeroing_complete() ? tran(&idle) : Q_RET_HANDLED;
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&homing);
        break;
    }

    return status_;
}

// running::busy::command_wait
Q_STATE_DEF(TMC429Mgr, command_wait) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            status_ = tran(&idle);
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&busy);
        break;
    }

    return status_;
}

// running::busy::config_wait
Q_STATE_DEF(TMC429Mgr, config_wait) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
        config_received_packets_count = 0;
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG:
        if (is_transaction_good(static_cast<const TransactionCompleteEvent *>(e))) {
            config_received_packets_count++;
            record_status(static_cast<const TransactionCompleteEvent *>(e));
            // TODO: Make this not a hard-coded value.
            if (config_received_packets_count >= 6) {
                status_ = tran(&idle);
            }
            else {
                status_ = Q_RET_HANDLED;
            }
        }
        else {
            status_ = tran(&fault);
        }
        break;

    default:
        status_ = super(&busy);
        break;
    }

    return status_;
}

// running::busy::query_motor_feedback
Q_STATE_DEF(TMC429Mgr, query_motor_feedback) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	SendQueries();
    	status_ = Q_RET_HANDLED;
    	break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    case TMC429_TRANSACTION_COMPLETE_SIG: {
    	auto evt = static_cast<const TransactionCompleteEvent *>(e);
        if (is_transaction_good(evt)) {
            record_status(evt);
            query_state.collect_response(SPIBufToTMC429Buf(evt->read_buf));
            if (query_state.queries_complete()) {
            	QSpyPrintMotorFeedback();
            	PublishMotorFeedback();
            	status_ = tran(&idle);
            }
            else {
            	status_ = Q_RET_HANDLED;
            }
        }
        else {
            status_ = tran(&fault);
        }
        break;
    }

    default:
        status_ = super(&busy);
        break;
    }

    return status_;
}

Q_STATE_DEF(TMC429Mgr, fault) {
    QP::QState status_;

    switch (e->sig) {
    case Q_ENTRY_SIG:
    	COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("TMC429Mgr faulted.");
        status_ = Q_RET_HANDLED;
        break;
    case Q_EXIT_SIG:
        status_ = Q_RET_HANDLED;
        break;

    default:
        status_ = super(&top);
        break;
    }

    return status_;
}

} // namespace TMC429
} // namespace COMMON
