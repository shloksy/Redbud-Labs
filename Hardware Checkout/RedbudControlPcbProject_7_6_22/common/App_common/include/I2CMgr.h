/**
 * @file I2CMgr.h
 *
 * @author awong
 *
 * QP Interface:
 * Subscribe:
 * - WRITE_TRANSACTION_SIG:
 *   Signal indicating a publisher wants to begin an I2C write transaction.
 *   Event contains I2C data to be written and recipient identification.
 * - READ_TRANSACTION_SIG:
 *    Signal indicating a publisher wants to begin an I2C read transaction.
 *   Event contains recipient identification.
 * Publish:
 * - {qp_complete_signal}
 *   This published signal comes from the I2C_WRITE_TRANSACTION_SIG or I2C_READ_TRANSACTION_SIG event.
 *   This is application specific and allows some form of unicasting to a particular active object.
 *
 * State Machine:
 * - idle
 *   No transaction is currently being handled.  Immediately acts on a received transaction.
 * - busy
 *   Currently writing or reading to I2C device.  Defers any incoming transactions until next time in idle state.
 *   - write_transaction_in_progress
 *     Sends write transaction and waits for write transaction complete event to be posted by ISR.
 *   - read_transaction_in_progress
 *     Sends read transaction and waits for read transaction complete event to be posted by ISR.
 */

#ifndef INCLUDE_I2CMGR_H_
#define INCLUDE_I2CMGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"
#include "qspy_bsp.h"

#include "InterruptEntry.h"

// Note: Q_DEFINE_THIS_MODULE("") must be defined before I2CMgr.h is included in a source file.

namespace COMMON {
namespace I2C {

// I2CTraits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct I2CTraits {
//    static constexpr enum_t WRITE_TRANSACTION_SIG = 0;
//    static constexpr enum_t READ_TRANSACTION_SIG = 0;
//
//    static constexpr I2C_HandleTypeDef *i2c_handle = nullptr;
//
//    // Depth of the internal defer/recall QP queue within I2CMgr.
//    static constexpr size_t TransactionQueueDepth = 0;
//    // Depth of the I2C data buffers used within for I2CMgr.
//    static constexpr size_t MaxI2CTransactionLength = 0;
//};

template<typename I2C_traits>
class I2CMgr : public QP::QActive, I2C_traits {
private:
    // Configuration:
    static constexpr size_t TransactionQueueDepth = I2C_traits::TransactionQueueDepth;
    static constexpr size_t MaxI2CTransactionLength = I2C_traits::MaxI2CTransactionLength;

    enum InternalSignals {
        TIMEOUT_SIG = MAX_PUB_SIG,
        WRITE_TRANSACTION_DONE_SIG,
        READ_TRANSACTION_DONE_SIG,
        ABORT_SIG,
        ERROR_SIG,
        TRANSACTION_ERROR_SIG,
    };

    QP::QEQueue I2CTransactionQueue;
    QP::QEvt const * I2CTransactionQSto[TransactionQueueDepth];

    struct I2CTransaction {
        enum_t qp_complete_signal;
        uint8_t i2c_addr;
        std::array<uint8_t, MaxI2CTransactionLength> buf;
        size_t transaction_len;
    };

    I2CTransaction CurrentTransaction;

    std::array<uint8_t, MaxI2CTransactionLength> empty_buf;

    QP::QTimeEvt m_timeEvt;

    struct TransactionErrorEvent : public QP::QEvt {
        ErrorCode error_code;
    };

public:
    I2CMgr() : QActive(Q_STATE_CAST(&I2CMgr::initial)),
    CurrentTransaction{},
    empty_buf{0},
    m_timeEvt(this, TIMEOUT_SIG, 0U) {
        I2CTransactionQueue.init(I2CTransactionQSto, Q_DIM(I2CTransactionQSto));

        // Note: Required that the i2c_handle won't be used before being initialized in main().
    }

    // NOTE: The following are called from an interrupt context!
    void I2CWriteTransactionComplete(InterruptEntry = {}) {
        postWriteTransactionDone();
    }
    void I2CReadTransactionComplete(InterruptEntry = {}) {
        postReadTransactionDone();
    }
    void I2CAbortComplete(InterruptEntry = {}) {
        postAbort();
    }
    void I2CError(InterruptEntry = {}) {
        postError();
    }

private:
    void setCurrentWriteTransaction(const WriteTransactionEvent * e);
    void setCurrentReadTransaction(const ReadTransactionEvent * e);
    void setCurrentTransaction(const enum_t qp_complete_signal,
                               const uint8_t i2c_addr,
                               const std::array<uint8_t, MaxI2CTransactionLength> &buf,
                               const size_t transaction_len);

    ErrorCode startWriteTransaction() {
        if (HAL_I2C_Master_Transmit_IT(I2C_traits::i2c_handle,
                CurrentTransaction.i2c_addr,
                CurrentTransaction.buf.data(),
                CurrentTransaction.transaction_len) == HAL_OK) {
            return NoError;
        }
        else {
            return PeripheralError;
        }
    }
    ErrorCode startReadTransaction() {
        if (HAL_I2C_Master_Receive_IT(I2C_traits::i2c_handle,
            CurrentTransaction.i2c_addr,
            CurrentTransaction.buf.data(),
            CurrentTransaction.transaction_len) == HAL_OK) {
            return NoError;
        }
        else {
            return PeripheralError;
        }
    }
    void WriteTransactionDoneEvent() {
        // TODO: For now, no handling of errors on the I2C transaction.
        publishTransactionCompleteEvent(CurrentTransaction.qp_complete_signal,
                ErrorCode::NoError,
                empty_buf,
                CurrentTransaction.transaction_len);
    }
    void ReadTransactionDoneEvent() {
        // TODO: For now, no handling of errors on the I2C transaction.
        publishTransactionCompleteEvent(CurrentTransaction.qp_complete_signal,
                ErrorCode::NoError,
                CurrentTransaction.buf,
                CurrentTransaction.transaction_len);
    }

    void publishErrorEvent(const enum_t qp_complete_signal,
                           const ErrorCode error_code) {
        publishTransactionCompleteEvent(qp_complete_signal,
                error_code,
                empty_buf,
                0);
    }
    void publishTransactionCompleteEvent(const enum_t qp_complete_signal,
                                         const ErrorCode error_code,
                                         const std::array<uint8_t, MaxI2CTransactionLength> &read_buf,
                                         const size_t transaction_len);

    void postTransactionError(const ErrorCode error_code) {
        auto e = Q_NEW(TransactionErrorEvent, TRANSACTION_ERROR_SIG);
        e->error_code = error_code;
        POST(e, this);
    }

    // NOTE: The following are called from an interrupt context!
    void postWriteTransactionDone() {
        POST(Q_NEW(QP::QEvt, WRITE_TRANSACTION_DONE_SIG), this);
    }
    void postReadTransactionDone() {
        POST(Q_NEW(QP::QEvt, READ_TRANSACTION_DONE_SIG), this);
    }
    void postAbort() {
        POST(Q_NEW(QP::QEvt, ABORT_SIG), this);
    }
    void postError() {
        POST(Q_NEW(QP::QEvt, ERROR_SIG), this);
    }

protected:
    Q_STATE_DEF_INTERNAL(I2CMgr, initial) {
        (void)e;

        subscribe(I2C_traits::WRITE_TRANSACTION_SIG);
        subscribe(I2C_traits::READ_TRANSACTION_SIG);

        return tran(&idle);
    }

    Q_STATE_DEF_INTERNAL(I2CMgr, idle) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                // On entry to &idle, attempt to recall an event from the internal queue.  The event at the head of the
                // internal queue will be posted LIFO of the AO queue (which is what we want).  This preserves the original
                // order of the transaction events that were passed to I2CMgr.
                recall(&I2CTransactionQueue);
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

                // If we get a transaction request, and we aren't in the &busy state, go ahead and begin the transaction.
            case I2C_traits::WRITE_TRANSACTION_SIG:
                setCurrentWriteTransaction(static_cast<const WriteTransactionEvent *>(e));
                status_ = tran(&write_transaction_in_progress);
                break;

                // If we get a transaction request, and we aren't in the &busy state, go ahead and begin the transaction.
            case I2C_traits::READ_TRANSACTION_SIG:
                setCurrentReadTransaction(static_cast<const ReadTransactionEvent *>(e));
                status_ = tran(&read_transaction_in_progress);
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

    Q_STATE_DEF_INTERNAL(I2CMgr, busy) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

                // If we get a transaction request, and we are in the &busy state, defer it to our internal QP queue.
                // Once we are not busy, we will grab transactions from the internal queue first.
            case I2C_traits::WRITE_TRANSACTION_SIG:
            case I2C_traits::READ_TRANSACTION_SIG:
                defer(&I2CTransactionQueue, e);
                status_ = Q_RET_HANDLED;
                break;

            case ABORT_SIG:
                // TODO: Testing: So far it seems like we never get here...
                Q_ASSERT_ID(40, false);

                // TODO: Graceful recovery from abort event is not handled correctly yet.

                publishErrorEvent(CurrentTransaction.qp_complete_signal,
                                  GenericError);
                status_ = tran(&idle);
                break;

            case ERROR_SIG:
                COMMON::QSpy::uartDisplayString("I2CMgr error.");
                publishErrorEvent(CurrentTransaction.qp_complete_signal,
                                  GenericError);
                status_ = tran(&idle);
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

    // busy::write_transaction_in_progress
    Q_STATE_DEF_INTERNAL(I2CMgr, write_transaction_in_progress) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                auto error_code = startWriteTransaction();
                if (error_code != NoError) {
                    postTransactionError(error_code);
                }
                status_ = Q_RET_HANDLED;
                break;
            }

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case WRITE_TRANSACTION_DONE_SIG:
                WriteTransactionDoneEvent();
                status_ = tran(&idle);
                break;

            case TRANSACTION_ERROR_SIG:
                publishErrorEvent(CurrentTransaction.qp_complete_signal,
                                  static_cast<const TransactionErrorEvent *>(e)->error_code);
                status_ = tran(&idle);
                break;

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }

    // busy::read_transaction_in_progress
    Q_STATE_DEF_INTERNAL(I2CMgr, read_transaction_in_progress) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG: {
                auto error_code = startReadTransaction();
                if (error_code != NoError) {
                    postTransactionError(error_code);
                }
                status_ = Q_RET_HANDLED;
                break;
            }

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case READ_TRANSACTION_DONE_SIG:
                ReadTransactionDoneEvent();
                status_ = tran(&idle);
                break;

            case TRANSACTION_ERROR_SIG:
                publishErrorEvent(CurrentTransaction.qp_complete_signal,
                                  static_cast<const TransactionErrorEvent *>(e)->error_code);
                status_ = tran(&idle);
                break;

            default:
                status_ = super(&busy);
                break;
        }

        return status_;
    }
};

template<typename I2C_traits>
void I2CMgr<I2C_traits>::publishTransactionCompleteEvent(const enum_t qp_complete_signal,
                                                         const ErrorCode error_code,
                                                         const std::array<uint8_t, MaxI2CTransactionLength> &read_buf,
                                                         const size_t transaction_len) {
    Q_ASSERT_ID(5, qp_complete_signal <= MAX_PUB_SIG);
    Q_ASSERT_ID(6, qp_complete_signal >= QP::Q_USER_SIG);

    QS_BEGIN_ID(I2CMgrMsg, getPrio())
        QS_STR("Complete:");
    QS_END()
    QS_BEGIN_ID(I2CMgrMsg, getPrio())
        QS_U32(1, qp_complete_signal);
        QS_U32(2, error_code);
        for (size_t i = 0; i < transaction_len; i++) {
            QS_U32_HEX(2, read_buf[i]);
        }
        QS_U32(1, transaction_len);
    QS_END()

    auto e = Q_NEW(TransactionCompleteEvent, qp_complete_signal);
    e->read_buf = read_buf;
    e->transaction_len = transaction_len;
    e->error_code = error_code;
    QP::QF::PUBLISH(e, this);
}

template<typename I2C_traits>
void I2CMgr<I2C_traits>::setCurrentWriteTransaction(const WriteTransactionEvent * e) {
    QS_BEGIN_ID(I2CMgrMsg, getPrio())
        QS_STR("Write:");
    QS_END()
    setCurrentTransaction(e->qp_complete_signal,
            e->i2c_addr,
            e->write_buf,
            e->transaction_len);
}

template<typename I2C_traits>
void I2CMgr<I2C_traits>::setCurrentReadTransaction(const ReadTransactionEvent * e) {
    QS_BEGIN_ID(I2CMgrMsg, getPrio())
        QS_STR("Read:");
    QS_END()
    setCurrentTransaction(e->qp_complete_signal,
            e->i2c_addr,
            empty_buf,
            e->transaction_len);
}

template<typename I2C_traits>
void I2CMgr<I2C_traits>::setCurrentTransaction(const enum_t qp_complete_signal,
                                               const uint8_t i2c_addr,
                                               const std::array<uint8_t, MaxI2CTransactionLength> &buf,
                                               const size_t transaction_len) {
    Q_ASSERT_ID(10, qp_complete_signal <= MAX_PUB_SIG);
    Q_ASSERT_ID(15, qp_complete_signal >= QP::Q_USER_SIG);
    Q_ASSERT_ID(20, transaction_len <= MaxI2CTransactionLength);
    Q_ASSERT_ID(25, transaction_len != 0);

    QS_BEGIN_ID(I2CMgrMsg, getPrio())
        QS_U32(1, qp_complete_signal);
        QS_U32_HEX(2, i2c_addr);
        for (size_t i = 0; i < transaction_len; i++) {
            QS_U32_HEX(2, buf[i]);
        }
        QS_U32(1, transaction_len);
    QS_END()

    // Copy write data into static buffer for use by the transaction.
    CurrentTransaction.qp_complete_signal = qp_complete_signal;
    CurrentTransaction.i2c_addr = i2c_addr;
    CurrentTransaction.buf = buf;
    CurrentTransaction.transaction_len = transaction_len;
}

} // namespace I2C
} // namespace COMMON

#endif /* INCLUDE_I2CMGR_H_ */
