/**
 * @file TTPVentusMgrApp.h
 *
 * @author awong
 */

#ifndef INC_TTPVENTUSMGRAPP_H_
#define INC_TTPVENTUSMGRAPP_H_

#include "usart.h"

static constexpr UART_HandleTypeDef *ttpventus_uart_handle = &huart4;

void TTPVentusUartTxCompleteCallback();
void TTPVentusUartRxCompleteCallback();
void TTPVentusUartErrorCallback();

#endif /* INC_TTPVENTUSMGRAPP_H_ */
