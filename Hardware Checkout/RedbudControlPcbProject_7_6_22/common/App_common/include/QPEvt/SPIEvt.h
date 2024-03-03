/**
 * @file SPIEvt.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_SPIEVT_H_
#define APP_COMMON_INCLUDE_QPEVT_SPIEVT_H_

#include "Config.h"

#include "qpcpp.hpp"

#include "stm32h7xx_hal.h"

#include <array>

#include "SPIPeriph.h"

namespace COMMON {
namespace SPI {

struct GpioPin {
    GPIO_TypeDef *reg;
    uint32_t pin;
};

enum ErrorCode : uint32_t {
    NoError = 0,
    QueueFull = 1,
    PeripheralError = 2,
    GenericError = 3,
    // Add more error codes as needed.
};

struct TransactionEvent : QP::QEvt {
    enum_t qp_complete_signal; // QP signal to be published upon completion of SPI transaction.
    GpioPin chip_select_pin;

    SpiConfig config;

    std::array<uint8_t, AppMaxSPITransactionLength> write_buf;
    size_t transaction_len;
};

// Generic SPI transaction complete event.  Signal is application specific.
struct TransactionCompleteEvent : QP::QEvt {
    std::array<uint8_t, AppMaxSPITransactionLength> read_buf;
    size_t transaction_len;

    ErrorCode error_code;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    TransactionEvent a;
    TransactionCompleteEvent b;
};

} // namespace SPI
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_SPIEVT_H_ */
