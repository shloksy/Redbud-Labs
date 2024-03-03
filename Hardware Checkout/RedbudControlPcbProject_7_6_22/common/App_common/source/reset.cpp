/*
 * reset.c
 *
 *  Created on: May 5, 2021
 *      Author: dsmoot
 */

#include "stm32h7xx_hal.h"

// Cause a software reset
void software_reset(void)
{
    //todo figure this out, got some confusing errors...

    HAL_NVIC_SystemReset();

    for (;;) { ; }
}
