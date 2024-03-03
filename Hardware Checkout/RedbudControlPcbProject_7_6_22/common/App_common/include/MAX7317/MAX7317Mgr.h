/**
 * @file MAX7317Mgr.h
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
 */

#ifndef APP_COMMON_INCLUDE_MAX7317_MAX7317MGR_H_
#define APP_COMMON_INCLUDE_MAX7317_MAX7317MGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"

#include "ArrayHelpers.h"
#include "bsp.h"
#include "qspy_bsp.h"
#include "QPEvt/SPIEvt.h"
#include "QPEvt/MAX7317Evt.h"

#include "MAX7317/MAX7317Protocol.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace MAX7317 {

// MAX7317Traits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct MAX7317Traits {
//    static constexpr enum_t SPI_TRANSACTION_SIG = 0;
//    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = 0;
//
//    static constexpr enum_t SET_OUTPUT_SIG = 0;
//
//    // Depth of the SPI data buffers used within for SPIMgr.
//    static constexpr size_t MaxSPITransactionLength = 0;
//
//    static constexpr size_t NumberOfDevicesDaisyChained = 0;
//
//    static constexpr GPIO_TypeDef * cs_gpio_port() {
//        return nullptr;
//    }
//    static constexpr uint16_t cs_gpio_pin = 0;
//};

template<typename MAX7317Traits>
class MAX7317Mgr : public QP::QActive, MAX7317Traits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        STARTUP_DELAY_SIG
    };

    using spi_trans_buf = std::array<uint8_t, MAX7317Traits::MaxSPITransactionLength>;
    static constexpr size_t max7317_chained_trans_buf_length = MAX7317Traits::NumberOfDevicesDaisyChained * max7317_transaction_length;
    using max7317_chained_trans_buf = std::array<uint8_t, max7317_chained_trans_buf_length>;

private:
    QP::QTimeEvt StartupDelayTimer;

    SetOutputEvent<MAX7317Traits::NumberOfDevicesDaisyChained> cached_event;

public:
    MAX7317Mgr()
    : QActive(Q_STATE_CAST(&MAX7317Mgr::initial)),
      StartupDelayTimer(this, STARTUP_DELAY_SIG, 0U),
      cached_event{}
    {
        ;
    }

private:
    void PublishSpiTransaction(const spi_trans_buf &write_buf, const size_t transaction_length) {
        auto e = Q_NEW(SPI::TransactionEvent, MAX7317Traits::SPI_TRANSACTION_SIG);
        e->qp_complete_signal = MAX7317Traits::SPI_TRANSACTION_COMPLETE_SIG;
        e->chip_select_pin.reg = MAX7317Traits::cs_gpio_port();
        e->chip_select_pin.pin = MAX7317Traits::cs_gpio_pin;
        e->config = max7317_spi_config;
        e->write_buf = write_buf;
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }

    void PublishSpiTransaction(const max7317_chained_trans_buf &write_buf) {
        spi_trans_buf spi_buf;
        auto transaction_length = write_buf.size();
        Q_ASSERT_ID(25, widen_array(spi_buf, write_buf, transaction_length));
        PublishSpiTransaction(spi_buf, transaction_length);
    }

    void PublishCachedEventToSpi() {
        max7317_chained_trans_buf chained_buf = {};
        for (size_t i = 0; i < MAX7317Traits::NumberOfDevicesDaisyChained; i++) {
            auto buf = Transaction::pack_enable_output(cached_event.port[i], cached_event.enable[i]);
            std::copy(buf.begin(), buf.end(), chained_buf.begin() + i * buf.size());
        }

        PublishSpiTransaction(chained_buf);
    }

    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

    bool is_transaction_good(const SPI::TransactionCompleteEvent * e) {
        // TODO: Check for other things?  Transaction length?
        return e->error_code == SPI::NoError;
    }

    max7317_chained_trans_buf SPIBufToMAX7317Buf(const spi_trans_buf &spi_buf, const size_t len) const {
        max7317_chained_trans_buf max7317_buf = {};
        Q_ASSERT_ID(30, narrow_array(max7317_buf, spi_buf, len));
        return max7317_buf;
    }
    max7317_chained_trans_buf SPIBufToMAX7317Buf(const spi_trans_buf &spi_buf) const {
        return SPIBufToHWHSCBuf(spi_buf, max7317_chained_trans_buf_length);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, initial) {
        (void)e;

        subscribe(MAX7317Traits::SPI_TRANSACTION_COMPLETE_SIG);
        subscribe(MAX7317Traits::SET_OUTPUT_SIG);

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, running) {
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
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, idle) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                // TODO: recall.
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7317Traits::SET_OUTPUT_SIG: {
                auto evt = static_cast<const SetOutputEvent<MAX7317Traits::NumberOfDevicesDaisyChained> *>(e);
                cached_event.port = evt->port;
                cached_event.enable = evt->enable;
                status_ = tran(&enable_output);
                break;
            }

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::busy
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, busy) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7317Traits::SET_OUTPUT_SIG:
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
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, startup) {
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
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, power_on_wait) {
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
                status_ = tran(&idle);
                break;

            default:
                status_ = super(&startup);
                break;
        }

        return status_;
    }

    //! running::busy::enable_output
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, enable_output) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                PublishCachedEventToSpi();
                status_ = Q_RET_HANDLED;
                break;
            }

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case MAX7317Traits::SPI_TRANSACTION_COMPLETE_SIG: {
                auto evt = static_cast<const SPI::TransactionCompleteEvent *>(e);
                if (is_transaction_good(evt)) {
                    status_ = tran(&idle);
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

    //! fault
    Q_STATE_DEF_INTERNAL(MAX7317Mgr, fault) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                COMMON::QSpy::uartDisplayString("MAX7317Mgr faulted.");
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

} // namespace MAX7317
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_MAX7317_MAX7317MGR_H_ */
