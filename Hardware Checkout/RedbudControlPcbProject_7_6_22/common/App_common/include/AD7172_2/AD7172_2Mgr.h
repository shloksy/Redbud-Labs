/**
 * @file AD7172_2Mgr.h
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

#ifndef APP_COMMON_INCLUDE_AD7172_2_AD7172_2MGR_H_
#define APP_COMMON_INCLUDE_AD7172_2_AD7172_2MGR_H_

#include "qpcpp.hpp"
#include "qphelper.h"

#include "ArrayHelpers.h"
#include "bsp.h"
#include "qspy_bsp.h"

#include "AD7172_2/AD7172_2Protocol.h"
#include "SPIPeriph.h"
#include "QPEvt/SPIEvt.h"

#include <stddef.h>
#include <array>
#include <cmath>

namespace COMMON {
namespace AD7172_2 {

// AD7172_2Traits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct AD7172_2Traits {
//    static constexpr enum_t SPI_TRANSACTION_SIG = 0;
//
//    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = 0;
//
//    // Depth of the I2C data buffers used within for I2CMgr.
//    static constexpr size_t MaxSPITransactionLength = 0;
//
//    static constexpr GPIO_TypeDef * cs_gpio_port() {
//        return nullptr;
//    }
//    static constexpr uint16_t cs_gpio_pin = 0;
//};

template<typename AD7172Traits>
class AD7172_2Mgr : public QP::QActive, AD7172Traits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG
    };

    using spi_trans_buf = std::array<uint8_t, AD7172Traits::MaxSPITransactionLength>;

public:
    AD7172_2Mgr()
    : QActive(Q_STATE_CAST(&AD7172_2Mgr::initial))
    {
        ;
    }

private:
    //! @todo This should work for now, but the array copy is copying extra since we can now have different
    //! transaction lengths (unlike TMC429 where this was copied from).
    void PublishSpiTransaction(ad7172_trans_buf &write_buf, const size_t transaction_length) {
        auto e = Q_NEW(SPI::TransactionEvent, AD7172Traits::SPI_TRANSACTION_SIG);
        e->qp_complete_signal = AD7172Traits::SPI_TRANSACTION_COMPLETE_SIG;
        e->chip_select_pin.reg = AD7172Traits::cs_gpio_port();
        e->chip_select_pin.pin = AD7172Traits::cs_gpio_pin;
        e->config = ad7172_spi_config;
        std::copy(write_buf.begin(), write_buf.end(), e->write_buf.begin());

        Q_ASSERT_ID(10, transaction_length < e->write_buf.size());
        e->transaction_len = transaction_length;
        QP::QF::PUBLISH(e, this);
    }
    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

    bool is_read_version_correct(const SPI::TransactionCompleteEvent * e) {
        if (is_transaction_good(e)) {
            return parse_transaction::is_read_version_correct(SPIBufToAD7172Buf(e->read_buf));
        }
        else {
            return false;
        }
    }

    bool is_transaction_good(const SPI::TransactionCompleteEvent * e) {
        //! @todo Check for other things?  Transaction length?
        return e->error_code == SPI::NoError;
    }

    ad7172_trans_buf SPIBufToAD7172Buf(const spi_trans_buf &spi_buf, const size_t len) const {
        ad7172_trans_buf ad7172_buf = {};

        // Note: Q_ASSERT_ID() does not like a template function as its test_ parameter.
        auto success = narrow_array(ad7172_buf, spi_buf, len);
        Q_ASSERT_ID(20, success);

        return ad7172_buf;
    }
    ad7172_trans_buf SPIBufToAD7172Buf(const spi_trans_buf &spi_buf) const {
        return SPIBufToAD7172Buf(spi_buf, ad7172_transaction_length);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, initial) {
        (void)e;

        subscribe(AD7172Traits::SPI_TRANSACTION_COMPLETE_SIG);

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, running) {
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
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, idle) {
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
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, busy) {
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
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, startup) {
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
                status_ = tran(&send_reset);
                break;

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }

    //! running::busy::startup::send_reset
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, send_reset) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                ad7172_trans_buf buf;
                auto length = build_transaction::send_reset(buf);
                PublishSpiTransaction(buf, length);
                status_ = Q_RET_HANDLED;
                break;
            }
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case AD7172_2_TRANSACTION_COMPLETE_SIG:
                if (is_transaction_good(static_cast<const SPI::TransactionCompleteEvent *>(e))) {
                    status_ = tran(&check_id);
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

    //! running::busy::startup::check_id
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, check_id) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                ad7172_trans_buf buf;
                auto length = build_transaction::read_version(buf);
                PublishSpiTransaction(buf, length);
                status_ = Q_RET_HANDLED;
                break;
            }
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case AD7172_2_TRANSACTION_COMPLETE_SIG:
                if (is_read_version_correct(static_cast<const SPI::TransactionCompleteEvent *>(e))) {
                    status_ = tran(&idle);
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
    Q_STATE_DEF_INTERNAL(AD7172_2Mgr, fault) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                COMMON::QSpy::uartDisplayString("AD7172_2Mgr faulted.");
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

} // namespace AD7172_2
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_AD7172_2_AD7172_2MGR_H_ */
