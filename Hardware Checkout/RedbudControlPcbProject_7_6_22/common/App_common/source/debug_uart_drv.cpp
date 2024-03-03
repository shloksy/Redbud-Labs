/**
 * @file debug_uart_drv.c
 *
 * @author dsmoot
 * @author awong
 */

#include <stdint.h>

#include "usart.h"
#include "qpcpp.hpp"

#include "debug_uart_drv.h"

namespace COMMON {
namespace DebugUart {

static bool uart_ready_for_more_data = true;

static uint8_t ReceivedByte;

HAL_StatusTypeDef SendBufferOverUart(const uint8_t *buf, uint16_t buf_len, uint32_t timeout) {
    return HAL_UART_Transmit(uart_handle, buf, buf_len, timeout);
}

HAL_StatusTypeDef SendBufferOverUartWithInterrupt(const uint8_t *buf, uint16_t buf_len) {
    return HAL_UART_Transmit_IT(uart_handle, buf, buf_len);
}

void StartInterruptReceiveOfSingleByteOnUart()
{
    UART_Start_Receive_IT(uart_handle, &ReceivedByte, 1);
}

bool IsUartReadyToTxMore() {
    return uart_ready_for_more_data;
}

void TakeTxMutex() {
    uart_ready_for_more_data = false;
}

void UartTxCompleteCallback() {
    uart_ready_for_more_data = true;
}

void UartRxCompleteCallback() {
#ifdef Q_SPY
    QP::QS::rxPut(ReceivedByte);
#endif
    StartInterruptReceiveOfSingleByteOnUart();
}

void UartErrorCallback() {
    StartInterruptReceiveOfSingleByteOnUart();
}

} // namespace QSpy
} // namespace COMMON
