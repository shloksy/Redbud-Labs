/**
 * @file MAX7315Mgr.h
 *
 * @author awong
 *
 * QP Interface:
 * Subscribe:
 * TBD
 * Publish:
 * TBD
 *
 * State Machine:
 * TBD
 *
 * @todo This module assumes all pins are outputs for now.
 */

#ifndef APP_COMMON_INCLUDE_MAX7315_MAX7315MGR_H_
#define APP_COMMON_INCLUDE_MAX7315_MAX7315MGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"

#include "ArrayHelpers.h"
#include "bsp.h"
#include "qspy_bsp.h"
#include "QPEvt/I2CEvt.h"
#include "QPEvt/MAX7315Evt.h"

#include "MAX7315/MAX7315Protocol.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace MAX7315 {

// MAX7315Traits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct MAX7315Traits {
//    static constexpr enum_t I2C_WRITE_SIG = 0;
//    static constexpr enum_t I2C_READ_SIG = 0;
//    static constexpr enum_t I2C_COMPLETE_SIG = 0;
//
//    static constexpr enum_t SET_OUTPUT_SIG = 0;
//
//    // Depth of the I2C data buffers used within for I2CMgr.
//    static constexpr size_t MaxI2CTransactionLength = 0;
//
//    static constexpr uint8_t I2CAddress = 0;
//};

template<typename MAX7315Traits>
class MAX7315Mgr : public QP::QActive, MAX7315Traits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        STARTUP_DELAY_SIG
    };

    using i2c_trans_buf = std::array<uint8_t, MAX7315Traits::MaxI2CTransactionLength>;

private:
    QP::QTimeEvt StartupDelayTimer;

    uint8_t cached_output;

public:
    MAX7315Mgr()
    : QActive(Q_STATE_CAST(&MAX7315Mgr::initial)),
      StartupDelayTimer(this, STARTUP_DELAY_SIG, 0U),
      cached_output(0)
    {
        ;
    }

private:
    void PublishI2CWriteTransaction(const i2c_trans_buf &write_buf, const size_t transaction_length) {
        auto e = Q_NEW(I2C::WriteTransactionEvent, MAX7315Traits::I2C_WRITE_SIG);
        e->qp_complete_signal = MAX7315Traits::I2C_COMPLETE_SIG;
        e->i2c_addr = MAX7315Traits::I2CAddress;
        e->write_buf = write_buf;
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }

    void PublishI2CReadTransaction(const size_t transaction_length) {
        auto e = Q_NEW(I2C::ReadTransactionEvent, MAX7315Traits::I2C_READ_SIG);
        e->qp_complete_signal = MAX7315Traits::I2C_COMPLETE_SIG;
        e->i2c_addr = MAX7315Traits::I2CAddress;
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }

    void PublishI2CWriteTransaction(const max7315_trans_buf &write_buf, const size_t transaction_length) {
        i2c_trans_buf i2c_buf;
        Q_ASSERT_ID(25, widen_array(i2c_buf, write_buf, transaction_length));
        PublishI2CWriteTransaction(i2c_buf, transaction_length);
    }

    void PublishCachedOutputEventToI2C() {
        max7315_trans_buf buf;
        buf[0] = BlinkPhase0OutputsRegister;
        buf[1] = cached_output;
        PublishI2CWriteTransaction(buf, 2);
    }

    void PublishConfigurationToI2C() {
        max7315_trans_buf buf;
        buf[0] = PortsConfigurationRegister;
        buf[1] = MAX7315Traits::IsInputPinConfiguration;
        PublishI2CWriteTransaction(buf, 2);
    }

    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

    constexpr void SetCachedOutput(const int port_index, const bool enable) {
        if (enable) {
            cached_output |= 1 << port_index;
        }
        else {
            cached_output &= ~(1 << port_index);
        }
    }

    bool is_transaction_good(const I2C::TransactionCompleteEvent * e) {
        // TODO: Check for other things?  Transaction length?
        return e->error_code == I2C::NoError;
    }

    max7315_trans_buf I2CBufToMAX7315Buf(const i2c_trans_buf &i2c_buf, const size_t len) const {
        max7315_trans_buf max7315_buf = {};
        Q_ASSERT_ID(30, narrow_array(max7315_buf, i2c_buf, len));
        return max7315_buf;
    }
    max7315_trans_buf I2CBufToMAX7315Buf(const i2c_trans_buf &i2c_buf) const {
        return I2CBufToHWHSCBuf(i2c_buf, max7315_transaction_length);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, initial) {
        (void)e;

        subscribe(MAX7315Traits::I2C_COMPLETE_SIG);
        subscribe(MAX7315Traits::SET_OUTPUT_SIG);

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, running) {
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

    //! running::idle
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, idle) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                // @todo QP recall event.
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7315Traits::SET_OUTPUT_SIG: {
                auto evt = static_cast<const SetOutputEvent *>(e);
                if (evt->port_index < NumberOfPorts) {
                    SetCachedOutput(evt->port_index, evt->enable);
                    status_ = tran(&enable_output);
                }
                else {
                    status_ = Q_RET_HANDLED;
                    // @todo QP recall event.
                }
                break;
            }

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::busy
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, busy) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7315Traits::SET_OUTPUT_SIG:
                // TODO: defer.
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::busy::startup
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, startup) {
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
                status_ = tran(&power_on_wait);
                break;

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }

    //! running::busy::startup::power_on_wait
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, power_on_wait) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                StartupDelayTimer.armX(ResetDelayTime_ms / BSP::MsPerBspTick, 0);
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                StartupDelayTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            case STARTUP_DELAY_SIG:
                status_ = tran(&set_configuration);
                break;

            default:
                status_ = super(&startup);
                break;
        }

        return status_;
    }

    //! running::busy::startup::set_configuration
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, set_configuration) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                PublishConfigurationToI2C();
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7315Traits::I2C_COMPLETE_SIG: {
                auto evt = static_cast<const I2C::TransactionCompleteEvent *>(e);
                status_ = is_transaction_good(evt) ? tran(&idle) : tran(&fault);
                break;
            }

            default:
                status_ = super(&startup);
                break;
        }

        return status_;
    }

    //! running::busy::enable_output
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, enable_output) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                PublishCachedOutputEventToI2C();
                status_ = Q_RET_HANDLED;
                break;
            }

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7315Traits::I2C_COMPLETE_SIG: {
                auto evt = static_cast<const I2C::TransactionCompleteEvent *>(e);
                status_ = is_transaction_good(evt) ? tran(&idle) : tran(&fault);
                break;
            }

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }

    //! fault
    Q_STATE_DEF_INTERNAL(MAX7315Mgr, fault) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                COMMON::QSpy::uartDisplayString("MAX7315Mgr faulted.");
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
};

} // namespace MAX7315
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_MAX7315_MAX7315MGR_H_ */
