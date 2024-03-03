//============================================================================
// Product: DPP example, NUCLEO-H743ZI board, cooperative QV kernel
// Last updated for version 7.2.0
// Last updated on  2022-12-13
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"
#include "dpp.hpp"
#include "bsp.hpp"

/* STM32CubeH7 include files */
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo_144.h"
// add other drivers if necessary...

Q_DEFINE_THIS_FILE // define the name of this file for assertions

// namespace DPP *************************************************************
namespace DPP {

// Local-scope objects -------------------------------------------------------
static uint32_t l_rnd; // random seed

#ifdef Q_SPY

    QP::QSTimeCtr QS_tickTime_;
    QP::QSTimeCtr QS_tickPeriod_;

    // QS source IDs
    static QP::QSpyId const l_SysTick_Handler = { 0U };

    static UART_HandleTypeDef l_uartHandle;

    enum AppRecords { // application-specific trace records
        PHILO_STAT = QP::QS_USER,
        PAUSED_STAT,
        COMMAND_STAT,
        CONTEXT_SW
    };

#endif

// ISRs used in this project =================================================
extern "C" {

//............................................................................
void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {
    // state of the button debouncing, see below
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current;
    uint32_t tmp;

#ifdef Q_SPY
    {
        tmp = SysTick->CTRL; // clear SysTick_CTRL_COUNTFLAG
        QS_tickTime_ += QS_tickPeriod_; // account for the clock rollover
    }
#endif

    //QP::QTimeEvt::TICK_X(0U, &l_SysTick_Handler); // process time events for rate 0
    DPP::ticker0.POST(nullptr,  &l_SysTick_Handler);

    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    //
    current = BSP_PB_GetState(BUTTON_USER); // read the User button
    tmp = buttons.depressed; // save the debounced depressed buttons
    buttons.depressed |= (buttons.previous & current); // set depressed
    buttons.depressed &= (buttons.previous | current); // clear released
    buttons.previous   = current; // update the history
    tmp ^= buttons.depressed;     // changed debounced depressed
    if (tmp != 0U) {  // debounced user button state changed?
        if (buttons.depressed != 0U) { // user button depressed?
            static QP::QEvt const pauseEvt = { DPP::PAUSE_SIG, 0U, 0U};
            QP::QF::PUBLISH(&pauseEvt, &l_SysTick_Handler);
        }
        else { // the button is released
            static QP::QEvt const serveEvt = { DPP::SERVE_SIG, 0U, 0U};
            QP::QF::PUBLISH(&serveEvt, &l_SysTick_Handler);
        }
    }

    QV_ARM_ERRATUM_838869();
}

//............................................................................
void USART3_IRQHandler(void); // prototype
#ifdef Q_SPY
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the QF and is not disabled. Such ISRs cannot post or publish events.
//
void USART3_IRQHandler(void) {
    // is RX register NOT empty?
    if ((DPP::l_uartHandle.Instance->ISR & USART_ISR_RXNE_RXFNE) != 0) {
        uint32_t b = DPP::l_uartHandle.Instance->RDR;
        QP::QS::rxPut(b);
        DPP::l_uartHandle.Instance->ISR &= ~USART_ISR_RXNE_RXFNE; // clear int.
    }
    QV_ARM_ERRATUM_838869();
}
#endif // Q_SPY

} // extern "C"

// BSP functions =============================================================
// MPU setup for STM32H743ZI MCU
static void STM32H743ZI_MPU_setup(void) {
    // The following MPU configuration contains just a generic ROM
    // region (with read-only access) and NULL-pointer protection region.
    // Otherwise, the MPU will fall back on the background region (PRIVDEFENA).
    //
    static struct {
        std::uint32_t rbar;
        std::uint32_t rasr;
    } const mpu_setup[] = {

        { // region #0: Flash: base=0x0000'0000, size=512M=2^(28+1)
          0x00000000U                       // base address
              | MPU_RBAR_VALID_Msk          // valid region
              | (MPU_RBAR_REGION_Msk & 0U), // region #0
          (28U << MPU_RASR_SIZE_Pos)        // 2^(18+1) region
              | (0x6U << MPU_RASR_AP_Pos)   // PA:ro/UA:ro
              | (1U << MPU_RASR_C_Pos)      // C=1
              | MPU_RASR_ENABLE_Msk         // region enable
        },

        { // region #7: NULL-pointer: base=0x000'0000, size=128M=2^(26+1)
          // NOTE: this region extends to  0x080'0000, which is where
          // the ROM is re-mapped by STM32
          //
          0x00000000U                       // base address
              | MPU_RBAR_VALID_Msk          // valid region
              | (MPU_RBAR_REGION_Msk & 7U), // region #7
          (26U << MPU_RASR_SIZE_Pos)        // 2^(26+1)=128M region
              | (0x0U << MPU_RASR_AP_Pos)   // PA:na/UA:na
              | (1U << MPU_RASR_XN_Pos)     // XN=1
              | MPU_RASR_ENABLE_Msk         // region enable
        },
    };

    // enable the MemManage_Handler for MPU exception
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

    __DSB();
    MPU->CTRL = 0U; // disable the MPU */
    for (std::uint_fast8_t n = 0U; n < Q_DIM(mpu_setup); ++n) {
        MPU->RBAR = mpu_setup[n].rbar;
        MPU->RASR = mpu_setup[n].rasr;
    }
    MPU->CTRL = MPU_CTRL_ENABLE_Msk         // enable the MPU */
                | MPU_CTRL_PRIVDEFENA_Msk;  // enable background region */
    __ISB();
    __DSB();
}
//............................................................................
void BSP::init(void) {
    // setup the MPU...
    STM32H743ZI_MPU_setup();

    // NOTE: SystemInit() has been already called from the startup code
    // but SystemCoreClock needs to be updated
    //
    SystemCoreClockUpdate();

    SCB_EnableICache(); // Enable I-Cache
    SCB_EnableDCache(); // Enable D-Cache

    // Configure Flash prefetch and Instr. cache through ART accelerator
#if (ART_ACCLERATOR_ENABLE != 0)
    __HAL_FLASH_ART_ENABLE();
#endif // ART_ACCLERATOR_ENABLE

    // Configure the LEDs
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);

    // Configure the User Button in GPIO Mode
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

    //...
    BSP::randomSeed(1234U);

    // initialize the QS software tracing...
    if (!QS_INIT(nullptr)) {
        Q_ERROR();
    }

    // dictionaries...
    QS_OBJ_DICTIONARY(AO_Table);
    QS_OBJ_DICTIONARY(AO_Philo[0]);
    QS_OBJ_DICTIONARY(AO_Philo[1]);
    QS_OBJ_DICTIONARY(AO_Philo[2]);
    QS_OBJ_DICTIONARY(AO_Philo[3]);
    QS_OBJ_DICTIONARY(AO_Philo[4]);

    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
    QS_OBJ_DICTIONARY(&DPP::ticker0);
    QS_USR_DICTIONARY(PHILO_STAT);
    QS_USR_DICTIONARY(PAUSED_STAT);
    QS_USR_DICTIONARY(COMMAND_STAT);
    QS_USR_DICTIONARY(CONTEXT_SW);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_ALL_RECORDS); // all records
    QS_GLB_FILTER(-QP::QS_QF_TICK);    // exclude the clock tick
}
//............................................................................
void BSP::terminate(int16_t result) {
    Q_UNUSED_PAR(result);
}
//............................................................................
void BSP::ledOn(void) {
    BSP_LED_On(LED1);
}
//............................................................................
void BSP::ledOff(void) {
    BSP_LED_Off(LED1);
}
//............................................................................
void BSP::displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'e') {
        BSP_LED_On(LED1);
    }
    else {
        BSP_LED_Off(LED1);
    }

    QS_BEGIN_ID(PHILO_STAT, AO_Philo[n]->m_prio) // app-specific record begin
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()          // application-specific record end
}
//............................................................................
void BSP::displayPaused(uint8_t paused) {
    if (paused != 0U) {
        BSP_LED_On(LED2);
    }
    else {
        BSP_LED_Off(LED2);
    }
}
//............................................................................
uint32_t BSP::random(void) { // a very cheap pseudo-random-number generator
    // flating point code is to exercise the FPU
    double volatile x = 3.1415926F;
    x = x + 2.7182818F;

    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    //
    uint32_t rnd = l_rnd * (3U*7U*11U*13U*23U);
    l_rnd = rnd; // set for the next time

    return (rnd >> 8);
}
//............................................................................
void BSP::randomSeed(uint32_t seed) {
    l_rnd = seed;
}


} // namespace DPP


// namespace QP **************************************************************
namespace QP {

// QF callbacks ==============================================================
void QF::onStartup(void) {
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SysTick_Config(SystemCoreClock / DPP::BSP::TICKS_PER_SEC);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system
    NVIC_SetPriority(USART3_IRQn,  0U); // kernel unaware interrupt
    NVIC_SetPriority(SysTick_IRQn, QF_AWARE_ISR_CMSIS_PRI + 1U);
    // ...

    // enable IRQs...
#ifdef Q_SPY
    NVIC_EnableIRQ(USART3_IRQn); // UART interrupt used for QS-RX
#endif
}
//............................................................................
void QF::onCleanup(void) {
}

//............................................................................
void QV::onIdle(void) { // called with interrupts disabled, see NOTE01
    // toggle the User LED on and then off, see NOTE01
    BSP_LED_On(LED3);
    BSP_LED_On(LED3);

#ifdef Q_SPY
    QF_INT_ENABLE();
    QS::rxParse();  // parse all the received bytes

    if ((DPP::l_uartHandle.Instance->ISR & UART_FLAG_TXE) != 0U) {//TXE empty?
        uint16_t b;

        QF_INT_DISABLE();
        b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  // not End-Of-Data?
            DPP::l_uartHandle.Instance->TDR = (b & 0xFFU); // put into TDR
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    //
    // !!!CAUTION!!!
    // The WFI instruction stops the CPU clock, which unfortunately disables
    // the JTAG port, so the ST-Link debugger can no longer connect to the
    // board. For that reason, the call to __WFI() has to be used with CAUTION.
    //
    // NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    // reset the board, then connect with ST-Link Utilities and erase the part.
    // The trick with BOOT(0) is it gets the part to run the System Loader
    // instead of your broken code. When done disconnect BOOT0, and start over.
    //QV_CPU_SLEEP(); // Wait-For-Interrupt
    QF_INT_ENABLE(); // just enable interrupts
#else
    QF_INT_ENABLE(); // just enable interrupts
#endif
}

//............................................................................
extern "C" Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    //
    // NOTE: add here your application-specific error handling
    //
    (void)module;
    (void)loc;
    QS_ASSERTION(module, loc, static_cast<uint32_t>(10000U));

#ifndef NDEBUG
    // light up LED
    BSP_LED_On(LED1);
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif

    NVIC_SystemReset();
}

// QS callbacks ==============================================================
#ifdef Q_SPY
//............................................................................
bool QS::onStartup(void const *arg) {
    static uint8_t qsTxBuf[2*1024]; // buffer for QS transmit channel
    static uint8_t qsRxBuf[100];    // buffer for QS receive channel

    initBuf  (qsTxBuf, sizeof(qsTxBuf));
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    DPP::l_uartHandle.Instance        = USART3;
    DPP::l_uartHandle.Init.BaudRate   = 115200;
    DPP::l_uartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    DPP::l_uartHandle.Init.StopBits   = UART_STOPBITS_1;
    DPP::l_uartHandle.Init.Parity     = UART_PARITY_NONE;
    DPP::l_uartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    DPP::l_uartHandle.Init.Mode       = UART_MODE_TX_RX;
    DPP::l_uartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&DPP::l_uartHandle) != HAL_OK) {
        return false; // return failure
    }

    // Set UART to receive 1 byte at a time via interrupt
    HAL_UART_Receive_IT(&DPP::l_uartHandle, (uint8_t *)qsRxBuf, 1);
    // NOTE: wait till QF::onStartup() to enable UART interrupt in NVIC

    DPP::QS_tickPeriod_ = SystemCoreClock / DPP::BSP::TICKS_PER_SEC;
    DPP::QS_tickTime_ = DPP::QS_tickPeriod_; // to start the timestamp at zero

    return true; // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {  // NOTE: invoked with interrupts DISABLED
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { // not set?
        return DPP::QS_tickTime_ - static_cast<QSTimeCtr>(SysTick->VAL);
    }
    else { // the rollover occured, but the SysTick_ISR did not run yet
        return DPP::QS_tickTime_ + DPP::QS_tickPeriod_
               - static_cast<QSTimeCtr>(SysTick->VAL);
    }
}
//............................................................................
void QS::onFlush(void) {
    uint16_t b;

    QF_INT_DISABLE();
    while ((b = getByte()) != QS_EOD) { // while not End-Of-Data...
        QF_INT_ENABLE();
        // while TXE not empty
        while ((DPP::l_uartHandle.Instance->ISR & UART_FLAG_TXE) == 0U) {
        }
        DPP::l_uartHandle.Instance->TDR = (b & 0xFFU); // put into TDR
        QF_INT_DISABLE();
    }
    QF_INT_ENABLE();
}
//............................................................................
//! callback function to reset the target (to be implemented in the BSP)
void QS::onReset(void) {
    NVIC_SystemReset();
}
//............................................................................
//! callback function to execute a user command (to be implemented in BSP)
extern "C" void assert_failed(char const *module, int loc);
void QS::onCommand(uint8_t cmdId, uint32_t param1,
                   uint32_t param2, uint32_t param3)
{
    (void)cmdId;
    (void)param1;
    (void)param2;
    (void)param3;

    QS_BEGIN_ID(DPP::COMMAND_STAT, 0U) // app-specific record
        QS_U8(2, cmdId);
        QS_U32(8, param1);
    QS_END()
}

#endif // Q_SPY
//----------------------------------------------------------------------------

} // namespace QP

extern "C" {
//............................................................................
#ifdef QF_ON_CONTEXT_SW
// NOTE: the context-switch callback is called with interrupts DISABLED
void QF_onContextSw(QP::QActive *prev, QP::QActive *next) {
    if ((prev != &DPP::ticker0) && (next != &DPP::ticker0)) {
        QS_BEGIN_NOCRIT(DPP::CONTEXT_SW, 0U) // no critical section!
            QS_OBJ(prev);
            QS_OBJ(next);
        QS_END_NOCRIT()
    }
}
#endif // QF_ON_CONTEXT_SW

} // extern "C"

//============================================================================
// NOTE1:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
// macros or any other QF/QK services. These ISRs are "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call any QF services. In particular they
// can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//

