/**
 * @file TMC429Mgr.cpp
 *
 * @author awong
 *
 * Module to provide some separation between TMC429Mgr methods and state machine.
 * Note: QP AO state machine can be found in TMC429MgrSM.
 */

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qspy_bsp.h"

#include "TMC429Mgr.h"
#include "TMC429Protocol.h"

#include <array>
#include <algorithm>

Q_DEFINE_THIS_MODULE("TMC429Mgr")

namespace COMMON {
namespace TMC429 {
using namespace COMMON::SPI;

void TMC429Mgr::PublishSpiTransaction(const tmc429_trans_buf &write_buf) {
    Q_ASSERT_ID(5, write_buf.size() <= AppMaxSPITransactionLength);

    //! @todo templated signal.
    auto e = Q_NEW(TransactionEvent, SPI2_TRANSACTION_SIG);
    e->qp_complete_signal = TMC429_TRANSACTION_COMPLETE_SIG;
    e->chip_select_pin = chip_select_pin;
    e->config = tmc429_spi_config;
    std::copy(write_buf.begin(), write_buf.end(), e->write_buf.begin());
    e->transaction_len = tmc429_transaction_length;
    QP::QF::PUBLISH(e, this);
}

void TMC429Mgr::PublishMotorFeedback() {
    //! @todo templated signal.
    auto e = Q_NEW(MotorFeedbackEvent, TMC429_MOTOR_FEEDBACK_SIG);
    e->error_code = TMC429NoError;
    e->motor_index = query_state.motor_index;
    e->status = status;
    e->position = query_state.position;
    e->velocity = query_state.velocity;
    e->int_flags = query_state.int_flags;
    QP::QF::PUBLISH(e, this);
}

void TMC429Mgr::PublishMotorPositionCommand(const SetMotorPositionEvent * e) {
    auto write_buf = build_transaction::set_position_target(static_cast<StepperMotorIndex>(e->motor_index),
            e->x_target);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::PublishMotorVelocityCommand(const SetMotorVelocityEvent * e) {
    auto write_buf = build_transaction::set_velocity_target(static_cast<StepperMotorIndex>(e->motor_index),
            e->v_target);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::PublishMotorRawCommand(const RawCommandEvent * e) {
    auto write_buf = build_transaction::set_general_command(e->addr,
            e->byte1,
            e->byte2,
            e->byte3);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::PublishMotorModeCommand(const SetModeEvent * e) {
    // TODO: ref_conf is 0 for now.
    auto write_buf = build_transaction::set_config_mode(static_cast<StepperMotorIndex>(e->motor_index),
            e->mode,
            DefaultRefConf);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::PublishMotorCurrentScalingCommand(const SetCurrentScalingEvent * e) {
    auto write_buf = build_transaction::set_current_scaling(e->motor_index,
            e->is_agtat,
            e->is_aleat,
            e->is_v0,
            e->a_threshold);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::PublishHomingCommandComplete(const ErrorCode error_code) {
    auto e = Q_NEW(HomingCommandCompleteEvent, TMC429_HOMING_COMMAND_COMPLETE_SIG);
    e->error_code = error_code;
    QP::QF::PUBLISH(e, this);
}

void TMC429Mgr::PostStartOperation() {
    POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
}

void TMC429Mgr::SendInitConfig() {
    tmc429_trans_buf write_buf;

    write_buf = build_transaction::set_global_parameters(true, Clock2Divider, num_motor_drivers);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_if_configuration(if_config);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_velocity_target(StepperMotor1, 0);
    PublishSpiTransaction(write_buf);
    write_buf = build_transaction::set_velocity_target(StepperMotor2, 0);
    PublishSpiTransaction(write_buf);
    write_buf = build_transaction::set_velocity_target(StepperMotor3, 0);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_config_mode(StepperMotor1, VelocityMode, DefaultRefConf);
    PublishSpiTransaction(write_buf);
    write_buf = build_transaction::set_config_mode(StepperMotor2, VelocityMode, DefaultRefConf);
    PublishSpiTransaction(write_buf);
    write_buf = build_transaction::set_config_mode(StepperMotor3, VelocityMode, DefaultRefConf);
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::handleConfigMotorEvent(const TMC429::ConfigMotorEvent * e) {
    pmul_pdiv.calculate(e->config.acceleration_max, e->config.ramp_div, e->config.pulse_div);

    // TODO: Probably should just return or something here.
    Q_ASSERT_ID(10, (pmul_pdiv.get_p_mul() != -1));
    Q_ASSERT_ID(11, (pmul_pdiv.get_p_div() != -1));

    auto motor_index = static_cast<StepperMotorIndex>(e->motor_index);
    tmc429_trans_buf write_buf;

    // TODO: Generalize this.
    write_buf = build_transaction::set_interrupt_mask(motor_index, 0xFF);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_velocity_min(motor_index, e->config.velocity_min);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_velocity_max(motor_index, e->config.velocity_max);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_accel_param_and_step_rate(motor_index,
            e->config.pulse_div,
            e->config.ramp_div,
            e->config.ustep_resolution_selection);
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_deceleration_ramp(motor_index,
            pmul_pdiv.get_p_mul(),
            pmul_pdiv.get_p_div());
    PublishSpiTransaction(write_buf);

    write_buf = build_transaction::set_accel_max(motor_index, e->config.acceleration_max);
    PublishSpiTransaction(write_buf);

    // TODO: REF_CONF?
            write_buf = build_transaction::set_config_mode(motor_index,
                    static_cast<MotionModes>(e->config.ramp_mode),
                    DefaultRefConf);
            PublishSpiTransaction(write_buf);
}

bool TMC429Mgr::is_transaction_good(const TransactionCompleteEvent * e) {
    return (e->error_code == NoError) && (e->transaction_len == tmc429_transaction_length);
}

bool TMC429Mgr::is_read_version_correct(const TransactionCompleteEvent * e) {
    if (is_transaction_good(e)) {
        return build_transaction::is_read_version_correct(SPIBufToTMC429Buf(e->read_buf));
    }
    else {
        return false;
    }
}

void TMC429Mgr::record_status(const TransactionCompleteEvent * e) {
    status.unpack(e->read_buf[0]);
}

tmc429_trans_buf TMC429Mgr::SPIBufToTMC429Buf(const spi_trans_buf &spi_buf, const size_t len) const {
    tmc429_trans_buf tmc429_buf = {};
    Q_ASSERT_ID(20, narrow_array(tmc429_buf, spi_buf, len));
    return tmc429_buf;
}

tmc429_trans_buf TMC429Mgr::SPIBufToTMC429Buf(const spi_trans_buf &spi_buf) const {
    return SPIBufToTMC429Buf(spi_buf, tmc429_transaction_length);
}

void TMC429Mgr::SendDriverChainTableTransaction() {
    tmc429_trans_buf write_buf;
    driver_chain_state.build_next_datagram();
    PublishSpiTransaction(write_buf);
}

void TMC429Mgr::SendHomingCommands() {
    for (size_t motor_index = 0; motor_index < NumStepperMotors; motor_index++) {
        if (homing_state.homing_enabled[motor_index]) {
            SendHomingCommands(static_cast<StepperMotorIndex>(motor_index));
        }
    }
}

void TMC429Mgr::SendHomingCommands(const StepperMotorIndex motor_index) {
    tmc429_trans_buf write_buf;

    // Set velocity to 0.
    write_buf = build_transaction::set_velocity_target(motor_index, 0);
    PublishSpiTransaction(write_buf);
    homing_state.homing_commands_sent++;

    // Set to velocity mode.
    write_buf = build_transaction::set_config_mode(motor_index, VelocityMode, DefaultRefConf);
    PublishSpiTransaction(write_buf);
    homing_state.homing_commands_sent++;

    // Set velocity to homing speed.
    write_buf = build_transaction::set_velocity_target(motor_index, homing_state.homing_velocity[motor_index]);
    PublishSpiTransaction(write_buf);
    homing_state.homing_commands_sent++;
}

void TMC429Mgr::SendHomingZero() {
    for (size_t motor_index = 0; motor_index < NumStepperMotors; motor_index++) {
        if (homing_state.homing_enabled[motor_index]) {
            SendHomingZero(static_cast<StepperMotorIndex>(motor_index));
        }
    }
}

void TMC429Mgr::SendHomingZero(const StepperMotorIndex motor_index) {
    tmc429_trans_buf write_buf;

    // Set velocity to 0.
    write_buf = build_transaction::set_velocity_target(motor_index, 0);
    PublishSpiTransaction(write_buf);
    homing_state.zero_commands_sent++;

    // Set actual position to 0.
    write_buf = build_transaction::set_position_actual(motor_index, 0);
    PublishSpiTransaction(write_buf);
    homing_state.zero_commands_sent++;
}

void TMC429Mgr::SendQueries() {
    auto motor_index = query_state.motor_index;

    QueryPosition(motor_index);
    query_state.queries_sent++;
    QueryVelocity(motor_index);
    query_state.queries_sent++;
    QueryInterruptFlags(motor_index);
    query_state.queries_sent++;
}

void TMC429Mgr::HomingCommandAssert(const TMC429::HomingCommandEvent * e) {
    // TODO: For now, we're assuming that homing is to the left hand reference switch, so
    // velocity must be negative.
    for (auto i = 0; i < NumStepperMotors; i++) {
        Q_ASSERT_ID(50, !(e->homing_enabled[i] && (e->homing_velocity[i] >= 0)));
    }
}

void TMC429Mgr::QSpyPrintMotorFeedback() {
    QS_BEGIN_ID(TMC429MgrMsg, getPrio())
        QS_U32(0, query_state.motor_index);
        QS_U8(0, status.INT);
        QS_U8(0, status.CDGW);
        QS_U8(0, status.RS[0]);
        QS_U8(0, status.RS[1]);
        QS_U8(0, status.RS[2]);
        QS_U8(0, status.xEQt[0]);
        QS_U8(0, status.xEQt[1]);
        QS_U8(0, status.xEQt[2]);
        QS_U32(0, query_state.position);
        QS_I16(0, query_state.velocity);
        QS_U8(0, query_state.int_flags);
    QS_END()
}

} // namespace TMC429
} // namespace COMMON
