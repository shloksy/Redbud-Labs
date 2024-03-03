/**
  ******************************************************************************
  * @file      stm32_lock_user.h
  * @author    STMicroelectronics
  * @brief     User defined lock mechanisms
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __STM32_LOCK_USER_H__
#define __STM32_LOCK_USER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if STM32_LOCK_API != 1 /* Version of the implemented API */
#error stm32_lock_user.h not compatible with current version of stm32_lock.h
#endif

/* Private defines -----------------------------------------------------------*/
/** Initialize members in instance of <code>LockingData_t</code> structure */
#define LOCKING_DATA_INIT { 0, 0 }

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t flag; /**< Backup of PRIMASK.PM at nesting level 0 */
  uint8_t counter; /**< Nesting level */
} LockingData_t;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Initialize STM32 lock
  * @param lock The lock to init
  */
void stm32_lock_init(LockingData_t *lock);

/**
  * @brief Acquire STM32 lock
  * @param lock The lock to acquire
  */
void stm32_lock_acquire(LockingData_t *lock);

/**
  * @brief Release STM32 lock
  * @param lock The lock to release
  */
void stm32_lock_release(LockingData_t *lock);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __STM32_LOCK_USER_H__ */
