/**
 * @file stm32_lock_user.cpp
 *
 * @author awong
 *
 * Note:  This implementation enables ST's thread safe solution Strategy #1 in CubeMX.  The
 *        newlib_lock_glue.c and stm32_lock.h generated files are essential to tying in these user
 *        defined lock calls with newlib.
 *        The generated stm32_lock_user.h template was ported here and filled in with Strategy #2
 *        before the scheduler runs, and then calls Error_Handler() when locks occur after the scheduler
 *        is primed.
 *        The current implementation is intended to allow calls to thread-unsafe functions
 *        during initialization.  Once initialization is complete, and before the QP
 *        scheduler starts running, stm32_lock_disable() must be called.  This
 *        disables locking during runtime and calls Error_Handler() if newlib locking inadvertently
 *        occurs, notifying a developer of the offending software (generally malloc() or something that
 *        calls malloc()).
 */

#include <stdint.h>
#include <stdbool.h>

#include "stm32_lock.h"

static bool stm32_newlib_lock_assert_enabled;

static void LOCK_ASSERT(uint32_t id, bool test) {
	(void) id;

	if (!test) {
		Error_Handler();
	}
}

static void stm32_lock_init_during_startup(LockingData_t *lock) {
	STM32_LOCK_BLOCK_IF_NULL_ARGUMENT(lock);
	lock->flag = 0;
	lock->counter = 0;
}

void stm32_lock_init(LockingData_t *lock) {
	LOCK_ASSERT(5, !stm32_newlib_lock_assert_enabled);
	stm32_lock_init_during_startup(lock);
}

static void stm32_lock_acquire_during_startup(LockingData_t *lock) {
	uint8_t flag = (uint8_t)(__get_PRIMASK() & 0x1); /* PRIMASK.PM */
	__disable_irq();
	__DSB();
	__ISB();
	STM32_LOCK_BLOCK_IF_NULL_ARGUMENT(lock);
	if (lock->counter == 0)
	{
		lock->flag = flag;
	}
	else if (lock->counter == UINT8_MAX)
	{
		STM32_LOCK_BLOCK();
	}
	lock->counter++;
}

void stm32_lock_acquire(LockingData_t *lock) {
	LOCK_ASSERT(10, !stm32_newlib_lock_assert_enabled);
	stm32_lock_acquire_during_startup(lock);
}

static void stm32_lock_release_during_startup(LockingData_t *lock) {
	STM32_LOCK_BLOCK_IF_NULL_ARGUMENT(lock);
	if (lock->counter == 0)
	{
	STM32_LOCK_BLOCK();
	}
	lock->counter--;
	if (lock->counter == 0 && lock->flag == 0)
	{
	__enable_irq();
	}
}

void stm32_lock_release(LockingData_t *lock) {
	LOCK_ASSERT(15, !stm32_newlib_lock_assert_enabled);
	stm32_lock_release_during_startup(lock);
}

void enable_stm32_newlib_lock_assert() {
	stm32_newlib_lock_assert_enabled = true;
}
