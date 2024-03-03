/**
 * @file TTPVentusMgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for TTPVentusMgr.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "TTPVentusMgrApp.h"

#include "usart.h" // Contains extern of uart handle.

// Must be defined before *.h include.
Q_DEFINE_THIS_MODULE("TTPVentusMgrApp.h")

#include "TTPVentus/TTPVentusMgr.h"

using namespace COMMON::TTPV;

struct TTPVAppTraits {
    static constexpr UART_HandleTypeDef *uart_handle = ttpventus_uart_handle;
};

static TTPVentusMgr<TTPVAppTraits> l_TTPVentusMgr;
QP::QActive * const AO_TTPVentusMgr = &l_TTPVentusMgr;

void TTPVentusUartTxCompleteCallback() {
    l_TTPVentusMgr.UARTTxComplete();
}

void TTPVentusUartRxCompleteCallback() {
    l_TTPVentusMgr.UARTRxComplete();
}

void TTPVentusUartErrorCallback() {
    l_TTPVentusMgr.UARTError();
}
