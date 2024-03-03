/**
 * @file bsp.h
 *
 * @author awong
 *
 * This will either point to qk_bsp.cpp or qv_bsp.cpp depending on the scheduler used.
 */

#ifndef BSP_H_
#define BSP_H_

#include <stdint.h>

namespace COMMON {

class BSP {
public:
    static constexpr uint32_t MsPerBspTick = 1;
    static constexpr uint32_t BSP_TICKS_PER_SEC = 1000 / MsPerBspTick;
    static void init(void);
    static void onIdle();
    static void BSP_ledOff();
    static void BSP_ledOn();
    static uint32_t HAL_GetTick(void);
};

#ifdef Q_SPY
extern BSP l_BSP;
#endif

} //namespace common

#endif /* BSP_H_ */
