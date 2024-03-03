/**
 * @file TMC429Mgr.h
 *
 * @author awong
 */

#ifndef INCLUDE_TMC429MGR_H_
#define INCLUDE_TMC429MGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"

#include "ArrayHelpers.h"
#include "bsp.h"

#include "QPEvt/SPIEvt.h"

#include "TMC429Protocol.h"
#include "TMC429DriverChainState.h"
#include "PMulPDiv.h"
#include "TMC429Query.h"
#include "TMC429Homing.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace TMC429 {

/**
 * TMC429 QP active object.
 */
class TMC429Mgr : public QP::QActive {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        HOMING_PERIODIC_QUERY_SIG,
    };

    // Configuration:
    static constexpr size_t DeferredEventQueueDepth = 30;
    static constexpr float TMC429ClockFrequencyHz = 16000000.0; //! Clock frequency feeding the TMC429.
    static constexpr uint32_t HomingQueryPeriodMs = 50;

    // Constants:
    static constexpr uint32_t HomingQueryTicks = HomingQueryPeriodMs / COMMON::BSP::MsPerBspTick;

    //! Clock rate feeding the TMC429 in ns.
    static constexpr float TMC429ClockRate_ns = 1000000000.0 / TMC429ClockFrequencyHz;
    //! Setup time as reported by the TMC429 datasheet (tHDCSC).
    static constexpr uint32_t SetupTime_us =
            static_cast<uint32_t>(ceil(TMC429ClockRate_ns / 1000.0f * SetupTime_TMC429ClockTicks));
    //! Hold time as reported by the TMC429 datasheet (tSUCSC).
    static constexpr uint32_t HoldTime_us =
            static_cast<uint32_t>(ceil(TMC429ClockRate_ns / 1000.0f * HoldTime_TMC429ClockTicks));

    static constexpr uint8_t Clock2Divider = 7; //! Filling in of the CLK2_DIV register.  7 is recommended by TMC429 datasheet for TMC249.
    static constexpr uint8_t DefaultRefConf = 0x0; //! @todo Filling in various bits of the REF_CONF register by default for now.

    //! SPI configuration structure for TMC429.
    static constexpr COMMON::SPI::SpiConfig tmc429_spi_config = {
        //! setup_time_us
        SetupTime_us,
        //! hold_time_us
        HoldTime_us,
        //! clock_polarity
        clock_polarity,
        //! clock_phase
        clock_phase
    };

    using spi_trans_buf = std::array<uint8_t, AppMaxSPITransactionLength>;

private:
    QP::QTimeEvt HomingQueryTimer;

    QP::QEQueue DeferredEventQueue;
    QP::QEvt const * DeferredEventQSto[DeferredEventQueueDepth];

    SPI::GpioPin chip_select_pin;
    IfConfiguration if_config;

    StatusBits status;

    int config_received_packets_count;

    DriverChainTableState driver_chain_state;
    PMulPDiv pmul_pdiv;
    const LSMD_NumberMotorDrivers num_motor_drivers;

    QueryState query_state;
    HomingState homing_state;

public:
    //! Constructor
    TMC429Mgr(const SPI::GpioPin chip_select_pin,
              const tmc429_driver_chain_table &driver_chain_table,
              const LSMD_NumberMotorDrivers num_motors,
              const IfConfiguration config);

private:
    void PublishSpiTransaction(const tmc429_trans_buf &write_buf);
    void PublishStatus();
    void PublishMotorFeedback();
    void PublishMotorPositionCommand(const SetMotorPositionEvent * e);
    void PublishMotorVelocityCommand(const SetMotorVelocityEvent * e);
    void PublishMotorRawCommand(const TMC429::RawCommandEvent * e);
    void PublishMotorModeCommand(const TMC429::SetModeEvent * e);
    void PublishMotorCurrentScalingCommand(const SetCurrentScalingEvent * e);
    void PublishHomingCommandComplete(const ErrorCode error_code);
    void PostStartOperation();

    void handleConfigMotorEvent(const ConfigMotorEvent * e);

    bool is_read_version_correct(const SPI::TransactionCompleteEvent * e);
    bool is_transaction_good(const SPI::TransactionCompleteEvent * e);
    void record_status(const SPI::TransactionCompleteEvent * e);

    void SendDriverChainTableTransaction();
    void SendInitConfig();
    void SendQueries();

    void QueryPosition(const StepperMotorIndex motor_index) {
        auto write_buf = build_transaction::get_position_actual(motor_index);
        PublishSpiTransaction(write_buf);
    }
    void QueryVelocity(const StepperMotorIndex motor_index) {
        auto write_buf = build_transaction::get_velocity_actual(motor_index);
        PublishSpiTransaction(write_buf);
    }
    void QueryInterruptFlags(const StepperMotorIndex motor_index) {
        auto write_buf = build_transaction::get_interrupt_flags(motor_index);
        PublishSpiTransaction(write_buf);
    }
    void QueryReferenceSwitches() {
        auto write_buf = build_transaction::get_reference_switches();
        PublishSpiTransaction(write_buf);
    }

    static void HomingCommandAssert(const TMC429::HomingCommandEvent * e);

    void SendHomingCommands();
    void SendHomingCommands(const StepperMotorIndex motor_index);

    void SendHomingZero();
    void SendHomingZero(const StepperMotorIndex motor_index);

    tmc429_trans_buf SPIBufToTMC429Buf(const spi_trans_buf &spi_buf, const size_t len) const;
    tmc429_trans_buf SPIBufToTMC429Buf(const spi_trans_buf &gt911_buf) const;

    void QSpyPrintMotorFeedback();

protected:
    //! initial
    Q_STATE_DECL(initial);
    //! running
    Q_STATE_DECL(running);

    //! running::idle
    //! @note Expectation is that there are only 2 sub-states of &running since &busy handles deferrals and
    //!       &idle handles recalling of events.
    Q_STATE_DECL(idle);
    //! running::busy
    Q_STATE_DECL(busy);

    //! running::busy::startup
    Q_STATE_DECL(startup);
    //! running::busy::startup::check_id
    Q_STATE_DECL(check_id);
    //! running::busy::startup::send_init_config
    Q_STATE_DECL(send_init_config);
    //! running::busy::startup::send_driver_chain_table
    Q_STATE_DECL(send_driver_chain_table);

    //! running::busy::homing
    Q_STATE_DECL(homing);
    //! running::busy::homing::send_homing_command
    Q_STATE_DECL(send_homing_command);
    //! running::busy::homing::homing_switch_wait
    Q_STATE_DECL(homing_switch_wait);
    //! running::busy::homing::set_homing_zero
    Q_STATE_DECL(send_homing_zero);

    //! running::busy::command_wait
    Q_STATE_DECL(command_wait);
    //! running::busy::config_wait
    Q_STATE_DECL(config_wait);
    //! running::busy::query_motor_feedback
    Q_STATE_DECL(query_motor_feedback);

    //! fault
    Q_STATE_DECL(fault);
};

} // namespace TMC429
} // namespace COMMON

#endif /* INCLUDE_TMC429MGR_H_ */
