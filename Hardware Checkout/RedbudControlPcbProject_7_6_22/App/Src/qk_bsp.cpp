/**
 * @file qk_bsp.cpp
 *
 * @author awong
 *
 * This module contains BSP elements for the QK scheduler.  This includes the system tick ISR
 * and all of the QF and QK callbacks.
 */

#include "qpcpp.hpp"

#include "bsp.h"
#include "qspy_bsp.h"

#include "reset.h"

#include "stm32h7xx_hal.h" // uwTick

#include <cstdio>

#include "stm32_lock_user_app.h"
#include "sys_app.h"

volatile bool qp_running = false;

extern "C" void HAL_IncTick(void) {
    QK_ISR_ENTRY(); // Stubbed in the QK port to Cortex M, but adding to be consistent.

    uwTick += uwTickFreq; //keeping HAL functionality at least for now.

    if (qp_running) {     // only call QP tick if QP is initialized
        QP::QF::TICK_X(0U, &COMMON::l_BSP); // time events for rate 0
    }

    // Commented out, but all ISRs must call one of these two macros.
    //QK_ARM_ERRATUM_838869(); // This should be called on exit of any non-QP-aware ISR.
    QK_ISR_EXIT(); // This must be called on exit of any QP-aware ISR in QK.
}

namespace QP {

void QF::onStartup(void) {
    enable_stm32_newlib_lock_assert();
    qp_running = true;
}

void QF::onCleanup(void) {
}

void QK::onIdle(void) {
    COMMON::QSpy::onIdle();
    COMMON::BSP::onIdle();
}

static char resetMsg[]  = "ERROR: Q_onAssert software reset!";

extern "C" void Q_onAssert(char const *module, int loc) {
    QS_ASSERTION(module, loc, static_cast<uint32_t>(10000U));

    COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(resetMsg);
    COMMON::QSpy::uartFlush();

    // Warning: Performs software reset!
    sys_app::sys_on_assert(module, loc);
}

} //namespace QP