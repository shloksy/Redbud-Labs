/**
 * @file UartApp.cpp
 *
 * @author awong
 *
 * Glue layer between UART interrupts and common/application.
 */

#include "Project.h"

#include "usart.h"

#include "debug_uart_drv.h"
#include "TTPVentusMgrApp.h"

static constexpr UART_HandleTypeDef *qspy_uart_handle = &huart1;

extern "C" {
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == qspy_uart_handle) {
        COMMON::DebugUart::UartTxCompleteCallback();
    }
    else if (huart == ttpventus_uart_handle) {
        TTPVentusUartTxCompleteCallback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == qspy_uart_handle) {
        COMMON::DebugUart::UartRxCompleteCallback();
    }
    else if (huart == ttpventus_uart_handle) {
        TTPVentusUartRxCompleteCallback();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart == qspy_uart_handle) {
        COMMON::DebugUart::UartErrorCallback();
    }
    else if (huart == ttpventus_uart_handle) {
        TTPVentusUartErrorCallback();
    }
}

}
