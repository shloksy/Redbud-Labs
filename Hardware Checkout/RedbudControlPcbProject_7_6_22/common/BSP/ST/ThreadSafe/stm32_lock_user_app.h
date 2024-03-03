/**
 * @file stm32_lock_user_app.h
 *
 * @author awong
 *
 * This header exposes enable_stm32_newlib_lock_assert() to the application and is more C++ friendly.
 */

#ifndef __STM32_LOCK_USER_APP_H__
#define __STM32_LOCK_USER_APP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  * @brief Enables asserting when newlib locks occur.  In general, this should be called right before
  *        interrupts or the scheduler is enabled.
  */
void enable_stm32_newlib_lock_assert();

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __STM32_LOCK_USER_APP_H__ */
