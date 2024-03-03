/**
 * @file debug_uart_drv.h
 *
 * @author dsmoot
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_QSPY_UART_DRV_H_
#define APP_COMMON_INCLUDE_QSPY_UART_DRV_H_

#include <stdint.h>

#include "Project.h"

#include "usart.h"

namespace COMMON {
namespace DebugUart {

// TODO: Hack to get around UART conflict with LCD screen.
#if COREH743I_C_PROJECT
static constexpr UART_HandleTypeDef *uart_handle = &huart1;
#else
#ifdef CORE_CM4
static constexpr UART_HandleTypeDef *uart_handle = &huart2;
#else
static constexpr UART_HandleTypeDef *uart_handle = &huart3;
#endif
#endif

HAL_StatusTypeDef SendBufferOverUart(const uint8_t *buf, uint16_t buf_len, uint32_t timeout);
HAL_StatusTypeDef SendBufferOverUartWithInterrupt(const uint8_t *buf, uint16_t buf_len);
void TakeTxMutex();
void StartInterruptReceiveOfSingleByteOnUart();
bool IsUartReadyToTxMore();

void UartTxCompleteCallback();
void UartRxCompleteCallback();
void UartErrorCallback();

} // namespace QSpy
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QSPY_UART_DRV_H_ */
