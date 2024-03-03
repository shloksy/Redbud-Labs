/**
 * @file sys_app.h
 *
 * @author jcochran
 *
 * @brief System-related functions.  Assertions.  Software reset.
 * Exception handlers.
 */

#ifndef SYS_APP_H_
#define SYS_APP_H_

#include <array>

namespace sys_app {

void init();

[[noreturn]] void software_reset();
void sys_assert(char const *module, int location, bool expression);
void sys_on_assert(char const *module, int location);

void reset_reason_capture();
void reset_reason_print();

void get_hw_uid(std::array<uint32_t, 3> &uid_copy);

}

/* System exception handlers */
/* These names override the default handlers that have weak linkage. */
extern "C" void HardFault_Handler();
extern "C" void MemManage_Handler();
extern "C" void BusFault_Handler();
extern "C" void UsageFault_Handler();

#endif /* SYS_APP_H_ */
