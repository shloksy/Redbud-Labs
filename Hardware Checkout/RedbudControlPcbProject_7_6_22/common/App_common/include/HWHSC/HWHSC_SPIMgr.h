/**
 * @file HWHSC_SPIMgr.h
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

#ifndef APP_COMMON_INCLUDE_HWHWSC_HWHSC_SPIMGR_H_
#define APP_COMMON_INCLUDE_HWHWSC_HWHSC_SPIMGR_H_

#include "qpcpp.hpp"
#include "qphelper.h"

#include "ArrayHelpers.h"
#include "bsp.h"
#include "qspy_bsp.h"
#include "SPIPeriph.h"
#include "QPEvt/SPIEvt.h"
#include "QPEvt/HWHSC_SPIEvt.h"

#include "HWHSC/HWHSC_SPIProtocol.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace HWHSC_SPI {

// HWHSC_SPITraits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct HWHSC_SPITraits {
//    static constexpr enum_t SPI_TRANSACTION_SIG = 0;
//    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = 0;
//
//    static constexpr enum_t SENSOR_DATA_SIG = 0;
//
//    // Depth of the SPI data buffers used within for SPIMgr.
//    static constexpr size_t MaxSPITransactionLength = 0;
//
//    // Period at which the sensor obtains data.
//    static constexpr uint32_t QuerySensorPeriodMs = 0;
//
//    static constexpr GPIO_TypeDef * cs_gpio_port() {
//        return nullptr;
//    }
//    static constexpr uint16_t cs_gpio_pin = 0;
//};

template<typename HWHSC_SPITraits>
class HWHSC_SPIMgr : public QP::QActive, HWHSC_SPITraits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        STARTUP_DELAY_SIG,
        PERIODIC_QUERY_SENSOR_SIG,
    };

    static constexpr uint32_t QuerySensorPeriodTicks = HWHSC_SPITraits::QuerySensorPeriodMs / COMMON::BSP::MsPerBspTick;

    using spi_trans_buf = std::array<uint8_t, HWHSC_SPITraits::MaxSPITransactionLength>;

private:
    QP::QTimeEvt StartupDelayTimer;
    QP::QTimeEvt PeriodicQuerySensorTimer;

    Transaction trans;

    SensorData cached_sensor_data;

public:
    HWHSC_SPIMgr()
    : QActive(Q_STATE_CAST(&HWHSC_SPIMgr::initial)),
      StartupDelayTimer(this, STARTUP_DELAY_SIG, 0U),
      PeriodicQuerySensorTimer(this, PERIODIC_QUERY_SENSOR_SIG, 0U),
      trans(),
      cached_sensor_data{}
    {
        ;
    }

private:
    void PublishSpiTransaction(const spi_trans_buf &write_buf, const size_t transaction_length) {
        auto e = Q_NEW(SPI::TransactionEvent, HWHSC_SPITraits::SPI_TRANSACTION_SIG);
        e->qp_complete_signal = HWHSC_SPITraits::SPI_TRANSACTION_COMPLETE_SIG;
        e->chip_select_pin.reg = HWHSC_SPITraits::cs_gpio_port();
        e->chip_select_pin.pin = HWHSC_SPITraits::cs_gpio_pin;
        e->config = hwhsc_spi_config;
        e->write_buf = write_buf;
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }

    void PublishSpiTransaction(const hwhsc_trans_buf &write_buf) {
        spi_trans_buf spi_buf;
        auto transaction_length = write_buf.size();
        Q_ASSERT_ID(25, widen_array(spi_buf, write_buf, transaction_length));
        PublishSpiTransaction(spi_buf, transaction_length);
    }

    void PublishCachedData() {
        auto e = Q_NEW(HWHSC_SPI::SensorEvent, HWHSC_SPITraits::SENSOR_DATA_SIG);
        e->data = cached_sensor_data;
        QP::QF::PUBLISH(e, this);
    }

    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

    bool is_transaction_good(const SPI::TransactionCompleteEvent * e) {
        //! @todo Check for other things?  Transaction length?
        return e->error_code == SPI::NoError;
    }

    void CacheData(const SPI::TransactionCompleteEvent * e) {
        cached_sensor_data = trans.parse(SPIBufToHWHSCBuf(e->read_buf));

        QS_BEGIN_ID(HWHSC_SPIMgrMsg, getPrio())
        QS_U8(0, cached_sensor_data.status);
        QS_U16(0, cached_sensor_data.pressure);
        QS_F32(2, cached_sensor_data.temperature);
        QS_END()
    }

    hwhsc_trans_buf SPIBufToHWHSCBuf(const spi_trans_buf &spi_buf, const size_t len) const {
        hwhsc_trans_buf hwhsc_init_buf = {};
        Q_ASSERT_ID(30, narrow_array(hwhsc_init_buf, spi_buf, len));
        return hwhsc_init_buf;
    }
    hwhsc_trans_buf SPIBufToHWHSCBuf(const spi_trans_buf &spi_buf) const {
        return SPIBufToHWHSCBuf(spi_buf, hwhsc_transaction_length);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, initial) {
        (void)e;

        subscribe(HWHSC_SPITraits::SPI_TRANSACTION_COMPLETE_SIG);

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, running) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                PeriodicQuerySensorTimer.armX(QuerySensorPeriodTicks, QuerySensorPeriodTicks);
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                PeriodicQuerySensorTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

    //! running::idle
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, idle) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case PERIODIC_QUERY_SENSOR_SIG:
                status_ = tran(&read_sensor);
                break;

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::busy
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, busy) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::busy::startup
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, startup) {
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
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, power_on_wait) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                StartupDelayTimer.armX(StartupTimeMax_ms, 0);
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

    //! running::busy::read_sensor
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, read_sensor) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                auto buf = trans.pack();
                PublishSpiTransaction(buf);
                status_ = Q_RET_HANDLED;
                break;
            }

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case HWHSC_SPITraits::SPI_TRANSACTION_COMPLETE_SIG: {
                auto evt = static_cast<const SPI::TransactionCompleteEvent *>(e);
                if (is_transaction_good(evt)) {
                    CacheData(evt);
                    PublishCachedData();
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
    Q_STATE_DEF_INTERNAL(HWHSC_SPIMgr, fault) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                COMMON::QSpy::uartDisplayString("HWHSC_SPIMgr faulted.");
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

} // namespace HWHSC_SPI
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_HWHWSC_HWHSC_SPIMGR_H_ */
