/**
 * @file SP160Mgr.h
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

#ifndef APP_COMMON_INCLUDE_SP160_SP160MGR_H_
#define APP_COMMON_INCLUDE_SP160_SP160MGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"

#include "ArrayHelpers.h"
#include "bsp.h"
#include "qspy_bsp.h"
#include "QPEvt/SPIEvt.h"

#include "SP160/SP160Protocol.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace SP160 {

// SP160Traits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct SP160Traits {
//    static constexpr enum_t SPI_TRANSACTION_SIG = 0;
//    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = 0;
//
//    // Depth of the SPI data buffers used within for SPIMgr.
//    static constexpr size_t MaxSPITransactionLength = 0;
//
//    static constexpr GPIO_TypeDef * cs_gpio_port() {
//        return nullptr;
//    }
//    static constexpr uint16_t cs_gpio_pin = 0;
//
//    static constexpr GPIO_TypeDef * reset_gpio_port() {
//        return nullptr;
//    }
//    static constexpr uint16_t reset_gpio_pin = 0;
//
//    static constexpr SP160Config config = {
//            PressureRange1_inH2O,
//            BWLimit25_Hz,
//            false,
//            AZModeStandard,
//            false,
//            0
//    };
//};

template<typename SP160Traits>
class SP160Mgr : public QP::QActive, SP160Traits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        STARTUP_DELAY_SIG,
    };

    using spi_trans_buf = std::array<uint8_t, SP160Traits::MaxSPITransactionLength>;

private:
    QP::QTimeEvt StartupDelayTimer;

    Transaction trans;
    ExtendedData extended_data;

public:
    SP160Mgr()
    : QActive(Q_STATE_CAST(&SP160Mgr::initial)),
      StartupDelayTimer(this, STARTUP_DELAY_SIG, 0U),
      trans(SP160Traits::config),
      extended_data{}
    {
        ;
    }

private:
    void PublishSpiTransaction(const spi_trans_buf &write_buf, const size_t transaction_length) {
        auto e = Q_NEW(SPI::TransactionEvent, SP160Traits::SPI_TRANSACTION_SIG);
        e->qp_complete_signal = SP160Traits::SPI_TRANSACTION_COMPLETE_SIG;
        e->chip_select_pin.reg = SP160Traits::cs_gpio_port();
        e->chip_select_pin.pin = SP160Traits::cs_gpio_pin;
        e->config = sp160_spi_config;
        e->write_buf = write_buf;
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }

    void PublishInitTransaction(const sp160_init_trans_buf &write_buf) {
        spi_trans_buf spi_buf;
        auto transaction_length = write_buf.size();
        Q_ASSERT_ID(20, widen_array(spi_buf, write_buf, transaction_length));
        PublishSpiTransaction(spi_buf, transaction_length);
    }

    void PublishSensorTransaction(const sp160_trans_buf &write_buf) {
        spi_trans_buf spi_buf;
        auto transaction_length = write_buf.size();
        Q_ASSERT_ID(25, widen_array(spi_buf, write_buf, transaction_length));
        PublishSpiTransaction(spi_buf, transaction_length);
    }

    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

    void RecordExtendedData(const SPI::TransactionCompleteEvent * e) {
        extended_data = trans.parse_init_transaction(SPIBufToSP160InitBuf(e->read_buf));
        QS_BEGIN_ID(SP160MgrMsg, getPrio())
        if (extended_data.model.back() == '\0') {
            QS_STR("Model=");
            QS_STR(extended_data.model.data());
        }
        else {
            QS_STR("ERROR: Non-null terminated Model=");
#ifdef Q_SPY
            for (const auto& c : extended_data.model) {
                QS_U32_HEX(2, c);
            }
#endif
        }
        QS_END()
        QS_BEGIN_ID(SP160MgrMsg, getPrio())
        QS_STR("Serial Number=");
#ifdef Q_SPY
        for (const auto& c : extended_data.serial_number) {
            QS_U32_HEX(2, c);
        }
#endif
        QS_END()
        QS_BEGIN_ID(SP160MgrMsg, getPrio())
        if (extended_data.build_number.back() == '\0') {
            QS_STR("Build Number=");
            QS_STR(extended_data.build_number.data());
        }
        else {
            QS_STR("ERROR: Non-null terminated Build Number=");
#ifdef Q_SPY
            for (const auto& c : extended_data.build_number) {
                QS_U32_HEX(2, c);
            }
#endif
        }
        QS_END()
    }

    bool is_extended_data_correct() {
        //! We could check for other models here if they are indeed the same:
        return extended_data.model == sp160_model;
    }

    bool is_transaction_good(const SPI::TransactionCompleteEvent * e) {
        //! @todo Check for other things?  Transaction length?
        return e->error_code == SPI::NoError;
    }

    sp160_init_trans_buf SPIBufToSP160InitBuf(const spi_trans_buf &spi_buf, const size_t len) const {
        sp160_init_trans_buf sp160_init_buf = {};
        Q_ASSERT_ID(30, narrow_array(sp160_init_buf, spi_buf, len));
        return sp160_init_buf;
    }
    sp160_init_trans_buf SPIBufToSP160InitBuf(const spi_trans_buf &spi_buf) const {
        return SPIBufToSP160InitBuf(spi_buf, sp160_init_transaction_length);
    }

    sp160_trans_buf SPIBufToSP160Buf(const spi_trans_buf &spi_buf, const size_t len) const {
        sp160_trans_buf sp160_buf = {};
        Q_ASSERT_ID(35, narrow_array(sp160_buf, spi_buf, len));
        return sp160_buf;
    }
    sp160_trans_buf SPIBufToSP160Buf(const spi_trans_buf &spi_buf) const {
        return SPIBufToSP160Buf(spi_buf, sp160_transaction_length);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(SP160Mgr, initial) {
        (void)e;

        subscribe(SP160Traits::SPI_TRANSACTION_COMPLETE_SIG);

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(SP160Mgr, running) {
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
    Q_STATE_DEF_INTERNAL(SP160Mgr, idle) {
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

    //! running::busy
    Q_STATE_DEF_INTERNAL(SP160Mgr, busy) {
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
    Q_STATE_DEF_INTERNAL(SP160Mgr, startup) {
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
                status_ = tran(&enable_wait);
                break;

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }

    //! running::busy::startup::enable_wait
    Q_STATE_DEF_INTERNAL(SP160Mgr, enable_wait) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                HAL_GPIO_WritePin(SP160Traits::reset_gpio_port(), SP160Traits::reset_gpio_pin, GPIO_PIN_SET);
                StartupDelayTimer.armX(ResetDelayTime_ms, 0);
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                StartupDelayTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            case STARTUP_DELAY_SIG:
                status_ = tran(&read_extended_data);
                break;

            default:
                status_ = super(&startup);
                break;
        }

        return status_;
    }

    //! running::busy::startup::read_extended_data
    Q_STATE_DEF_INTERNAL(SP160Mgr, read_extended_data) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                sp160_init_trans_buf buf = trans.pack_init_transaction();
                PublishInitTransaction(buf);
                status_ = Q_RET_HANDLED;
                break;
            }
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case SP160Traits::SPI_TRANSACTION_COMPLETE_SIG:
                if (is_transaction_good(static_cast<const SPI::TransactionCompleteEvent *>(e))) {
                    RecordExtendedData(static_cast<const SPI::TransactionCompleteEvent *>(e));
                    if (is_extended_data_correct()) {
                        status_ = tran(&idle);
                    }
                    else {
                        status_ = tran(&fault);
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

    //! fault
    Q_STATE_DEF_INTERNAL(SP160Mgr, fault) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                COMMON::QSpy::uartDisplayString("SP160Mgr faulted.");
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

} // namespace SP160
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_SP160_SP160MGR_H_ */
