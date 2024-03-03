/**
 * @file sys_app.cpp
 *
 * @author jcochran
 * @author awong
 */

#include "Config.h"

#include "stm32h7xx_hal.h"

#include "sys_app.h"

#include "qspy_bsp.h"

#include <cstdio>

// TODO: Extend actual UART handle to have this API.
namespace uart {
bool is_initialized() {
    return true;
}
}

namespace sys_app {

// Use a static character buffer for allocating strings to
// reduce stack usage in the exception handlers.
static char static_msg[128];

static void print_assert_message(char const *module, int location);

[[noreturn]] static void complete_fault() {
#if CONFIG_COMMON_ENABLE_BKPT_INST_AT_FAULTS
    __asm("  bkpt #0"); // breakpoint if debugger is attached.
#endif

    software_reset();
}

[[noreturn]] void software_reset() {
	// Request a system reset.
    HAL_NVIC_SystemReset();

	// Loop forever until reset.
	while (true)
		;
}

void sys_assert(char const *module, int location, bool expression) {
	if (expression)
		return;

	sys_on_assert(module, location);
}

void sys_on_assert(char const *module, int location) {
    if (uart::is_initialized()) {
        print_assert_message(module, location);
    }

    complete_fault();
}

static void print_assert_message(char const *module, int location) {
	std::snprintf(static_msg, sizeof(static_msg),
	              "ASSERT FAIL:  Module %s Location %d", module, location);
	COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(static_msg);
	COMMON::QSpy::uartFlush();
}

static uint32_t reset_reason;
void reset_reason_capture() {
	reset_reason = RCC->RSR;
	// Reset reset reason register.
	RCC->RSR |= RCC_RSR_RMVF;
}

void reset_reason_print() {
	if (uart::is_initialized()) {
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Reset Reasons:");
		if (reset_reason & RCC_RSR_LPWRRSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Low-power Reset");
		if (reset_reason & RCC_RSR_WWDG1RSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Window Watchdog Reset");
		if (reset_reason & RCC_RSR_IWDG1RSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Independent Watchdog Reset");
		if (reset_reason & RCC_RSR_SFTRSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Software Reset");
		if (reset_reason & RCC_RSR_PORRSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("POR/PDR Reset");
		if (reset_reason & RCC_RSR_PINRSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("NRST Pin Reset");
		if (reset_reason & RCC_RSR_BORRSTF)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("BOR Reset");
		if (reset_reason & RCC_RSR_D2RSTF)
            COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("D2 Domain Power Reset");
		if (reset_reason & RCC_RSR_D1RSTF)
            COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("D1 Domain Power Reset");
		if (reset_reason & RCC_RSR_CPURSTF)
            COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("CPU Reset");
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("----------");
	}
}

void get_hw_uid(std::array<uint32_t, 3> &uid_copy) {
    // Hardware register for unique ID.  The unique ID is a 96 bit number
    // in three words that is guaranteed to be unique for each MCU part.
    // The unique ID is not modifiable.
    auto hw_uid = reinterpret_cast<const volatile uint32_t*>(UID_BASE);

    uid_copy[0] = hw_uid[0];
    uid_copy[1] = hw_uid[1];
    uid_copy[2] = hw_uid[2];
}

} // namespace sys_app

extern "C" void HardFault_Handler() {
	// To get here we either had
	//  - Bus error on vector read
	//  - A fault during another fault

	if (uart::is_initialized()) {
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("FATAL:  HARD FAULT");
		if (SCB->HFSR & SCB_HFSR_VECTTBL_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Vector Table Read Failure");
		if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Fault in Fault");
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("----------");
	}

	sys_app::complete_fault();
}

extern "C" void MemManage_Handler() {
	// To get here we had MPU fault:
	//  - Bad instruction access
	//  - Bad data access
	//  - During exception stacking
	//  - During exception unstacking
	//  - During lazy floating-point state preservation.

	if (uart::is_initialized()) {
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("FATAL:  MPU FAULT");
		if (SCB->CFSR & SCB_CFSR_IACCVIOL_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On instruction access");
		if (SCB->CFSR & SCB_CFSR_DACCVIOL_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On data access");
		if (SCB->CFSR & SCB_CFSR_MUNSTKERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On exception unstacking");
		if (SCB->CFSR & SCB_CFSR_MSTKERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On exception stacking");
		if (SCB->CFSR & SCB_CFSR_MLSPERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On floating-point state preservation");

		if (SCB->CFSR & SCB_CFSR_MMARVALID_Msk) {
			std::snprintf(sys_app::static_msg, sizeof(sys_app::static_msg),
			              "At bad address: 0x%lx", SCB->MMFAR);
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(sys_app::static_msg);
		}
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("-----------------");
	}

	sys_app::complete_fault();
}

extern "C" void BusFault_Handler() {
	// To get here, we had some data bus error:
	//  - During exception stacking
	//  - During exception unstacking
	//  - During instruction prefetch
	//  - During lazy floating-point state preservation
	//
	// Class is either precise or imprecise

	if (uart::is_initialized()) {
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("FATAL:  BUS FAULT");
		if (SCB->CFSR & SCB_CFSR_IBUSERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("During instruction prefetch");
		if (SCB->CFSR & SCB_CFSR_UNSTKERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On exception unstacking");
		if (SCB->CFSR & SCB_CFSR_STKERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("On exception stacking");
		if (SCB->CFSR & SCB_CFSR_IMPRECISERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Imprecise fault");
		if (SCB->CFSR & SCB_CFSR_PRECISERR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Precise fault");

		if (SCB->CFSR & SCB_CFSR_BFARVALID_Msk) {
			std::snprintf(sys_app::static_msg,
			              sizeof(sys_app::static_msg),
			              "At bad address: 0x%lx",
			              SCB->BFAR);
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(sys_app::static_msg);
		}
	}

	sys_app::complete_fault();
}

extern "C" void UsageFault_Handler() {
	// To get here, we had a usage fault:
	// - Attempt to access non-existent coprocessor
	// - Bad opcode / undefined instruction
	// - Invalid instruction set state (bad Thumb transition)
	// - Invalid return from exception handler
	// - Unaligned memory access
	// - Division by zero (if configured to trap)

	if (uart::is_initialized()) {
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("FATAL:  USAGE FAULT");
		if (SCB->CFSR & SCB_CFSR_NOCP_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("No Coprocessor");
		if (SCB->CFSR & SCB_CFSR_INVPC_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Invalid return from exception");
		if (SCB->CFSR & SCB_CFSR_INVSTATE_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Invalid state usage");
		if (SCB->CFSR & SCB_CFSR_UNDEFINSTR_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Undefined instruction");
		if (SCB->CFSR & SCB_CFSR_UNALIGNED_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Unaligned access");
		if (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk)
			COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("Division by zero");
		COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy("-------------------");
	}

	sys_app::complete_fault();
}

