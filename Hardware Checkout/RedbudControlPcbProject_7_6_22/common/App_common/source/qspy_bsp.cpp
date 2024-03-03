/**
 * @file qspy_bsp.cpp
 *
 * @author dsmoot
 */

#include <stdint.h>
#include <cstring>
#include <array>

#include "Project.h"
#include "qpcpp.hpp"

#include "debug_uart_drv.h"
#include "reset.h"

#include "bsp.h"

namespace COMMON {
namespace QSpy {

#ifdef Q_SPY
static void MaybeSendTxBlock() {
    if (COMMON::DebugUart::IsUartReadyToTxMore() ) {
        uint16_t bytesToSend = 50;
        uint8_t const *bufPtr = QP::QS::getBlock(&bytesToSend);
        if (bufPtr != nullptr)
        {
            COMMON::DebugUart::TakeTxMutex();
            if (HAL_OK != COMMON::DebugUart::SendBufferOverUartWithInterrupt(const_cast<uint8_t *>(bufPtr), bytesToSend))
            {
                //todo do something!
            }
        }
    }
}
#endif

//! @todo (asw) Its not completely clear to me that this needs to be called with interrupts disabled.  Investigate whether
//! this is really required or not.
// Call with interrupts disabled.
void onIdle() {
#ifdef Q_SPY
    // Service the QSpy processing.
    QP::QS::rxParse();

    MaybeSendTxBlock();
#endif
}

bool uartDisplayText(const char* text, uint32_t length) {
#ifdef Q_SPY
    // TODO(jec):  Enumerate application-specific QS IDs in PlatformCommon / project.h.
    if (length < strlen(text)){
        const char *Err = "Unterminated / too long string error";
        QS_BEGIN_ID(COMMON::DisplayText, QP::QS_AP_ID)
            QS_STR(Err);
        QS_END()
    }
    else {
        QS_BEGIN_ID(COMMON::DisplayText, QP::QS_AP_ID)
            QS_STR(text);
        QS_END()
    }

#else

    //blocking transmit is probably a bad idea but we are just going to do it at startup I think
    COMMON::DebugUart::SendBufferOverUart((const uint8_t *)text, length, 100);
#endif
    return true;
}

bool uartDisplayString(const char* string) {
    return uartDisplayText(string, strlen(string));
}

bool uartDisplayStringWithNewLineExceptOnQspy(const char* string) {
    bool retVal;
    retVal = uartDisplayString(string);
    // Dubious retVal that's not really defined or set other than true.
    if (!retVal) {
        return retVal;
    }
#ifndef Q_SPY
    retVal = uartDisplayString("\r\n");
#endif

    return retVal;
}

void uartFlush() {
#ifdef Q_SPY
    QS_FLUSH();
#endif
}

}	//namespace QSPY
}	//namespace COMMON

namespace QP {


// QS callbacks =============================================================
// TODO:  Move some of this functionality into the ports, because the backend
// will be uart on the device, and something else on the win32 build.

#ifdef Q_SPY

bool QS::onStartup(const void* userData) {
    // Allocate static Tx and Rx buffers for QSpy.  Start at 4K each in the
    // absence of compelling reason otherwise.  If the buffers are too small,
    // you might lose data when there are big bursts.  If the buffers are too large
    // you might exceed the RAM on the device.
    static constexpr uint16_t buf_size = 4096;
    static std::array<uint8_t, buf_size> qspyRxBuf {};
    static std::array<uint8_t, buf_size> qspyTxBuf {};

    QS::initBuf(qspyTxBuf.data(), static_cast<uint16_t>(qspyTxBuf.size()));
    QS::rxInitBuf(qspyRxBuf.data(), static_cast<uint16_t>(qspyRxBuf.size()));

    COMMON::DebugUart::StartInterruptReceiveOfSingleByteOnUart();

    return true;
}

void QS::onCleanup() {
    // Cleanup not required.
}

// Flush QS buffer to host--This sends the contents of the QSpy buffer
// to the uart UART peripheral.
// Note: This can be called from ISR if QS_ASSERT, so need proper critical
// section wrappers
void QS::onFlush() {
    uint16_t bytesToSend = 50;

    uint32_t dummy_arg;
    (void)dummy_arg; // Note: On Cortex-M7 this isn't used.

    QF_CRIT_ENTRY(dummy_arg);

    uint8_t const *bufPtr = QS::getBlock(&bytesToSend);
    while (bufPtr != nullptr) {
        // In QK, there is a situation where the idle loop is transmitting and a higher priority AO is flushing.  When this
        // occurs, this SendBufferOverUart can collide with an idle loop transmit.  Since we're requiring a flush, we just
        // wait until the UART is not busy.
        while (COMMON::DebugUart::SendBufferOverUart(const_cast<uint8_t *>(bufPtr), bytesToSend, 100) == HAL_BUSY);
        bytesToSend = 50;
        bufPtr = QP::QS::getBlock(&bytesToSend);
    }
    QF_CRIT_EXIT(dummy_arg);
}


/* QS::onGetTime() provided by QUTest */
#ifndef Q_UTEST
QP::QSTimeCtr QS::onGetTime() {
    return COMMON::BSP::HAL_GetTick();
}
#endif

void QS::onReset() {
    software_reset();
}

void QS::onCommand(std::uint8_t cmdId,
                   std::uint32_t param1,
                   std::uint32_t param2,
                   std::uint32_t param3) {
    // todo	COMMON::TestProtocol::HandleCommand(cmdId, param1, param2, param3);
    return;
}

#endif /* Q_SPY */

#ifdef Q_UTEST
void QS::onTestLoop() {
    // This flag is apparently both a notification that the stack contains
    // this function, and also a keepGoing flag for this function.
    rxPriv_.inTestLoop = true;
    while (rxPriv_.inTestLoop) {
        rxParse();
        COMMON::QSpy::MaybeSendTxBlock();
    }

    rxPriv_.inTestLoop = true;
}

void QS::onTestEvt(QP::QEvt* e) {
    return;
}

#endif /* Q_UTEST */

} // namespace QP


