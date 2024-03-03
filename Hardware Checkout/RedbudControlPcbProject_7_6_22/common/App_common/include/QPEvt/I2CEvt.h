/**
 * @file I2CEvt.h
 *
 * @author awong
 *
 * Events for the I2CMgr AO.
 *
 * Note: Expected that the module that includes I2CEvt.h will define AppMaxI2CTransactionLength
 *       beforehand.
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_I2CEVT_H_
#define APP_COMMON_INCLUDE_QPEVT_I2CEVT_H_

#include <array>

namespace COMMON {
namespace I2C {

//! Public error codes reported from I2CMgr.
enum ErrorCode : uint32_t {
    NoError = 0, //! No error
    QueueFull = 1, //! I2C queue is full. Transaction was aborted.
    PeripheralError = 2, //! I2C peripheral had an error. Transaction was aborted.
    GenericError = 3, //! Other catch all that I2C peripheral had an error. Transaction was aborted.
};

//! Event that is published to the I2CMgr which contains information on an I2C write transaction.
//! Corresponds with application defined WRITE_TRANSACTION_SIG in I2C traits.
struct WriteTransactionEvent : public QP::QEvt {
    enum_t qp_complete_signal; //! QP signal to be published upon completion of I2C transaction.

    uint8_t i2c_addr; //! I2C address.

    std::array<uint8_t, AppMaxI2CTransactionLength> write_buf; //! Write buffer with data filled in up to transaction_len.
    size_t transaction_len; //! Transaction length of the I2C write.
};

//! Event that is published to the I2CMgr which contains information on an I2C read transaction.
//! Corresponds with application defined READ_TRANSACTION_SIG in I2C traits.
struct ReadTransactionEvent : public QP::QEvt {
    enum_t qp_complete_signal; //! QP signal to be published upon completion of I2C transaction.

    uint8_t i2c_addr; //! I2C address.

    size_t transaction_len; //! Transaction length of the I2C read.
};

//! Event that is published to the I2CMgr which contains information on an I2C read transaction.
//! Corresponds with application passed in qp_complete_signal from either WriteTransactionEvent or
//! ReadTransactionEvent.
struct TransactionCompleteEvent : public QP::QEvt {
    ErrorCode error_code; //! Reported error code from I2CMgr.

    std::array<uint8_t, AppMaxI2CTransactionLength> read_buf; //! Read buffer.  Not filled in on a write transaction.
    size_t transaction_len; //! Length of the read data in read_buf.
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    WriteTransactionEvent a; //! a
    ReadTransactionEvent b; //! b
    TransactionCompleteEvent c; //! c
};

} // namespace I2C
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_I2CEVT_H_ */
