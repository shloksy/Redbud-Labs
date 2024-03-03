/**
 * @file BLDCEvt.h
 *
 * @author awong
 */

#ifndef INC_QPEVT_BLDCEVT_H_
#define INC_QPEVT_BLDCEVT_H_

#include "qpcpp.hpp"

#include "stm32h7xx_hal.h"

#include <stdint.h>

namespace BLDC {

// SET_BLDC_SPEED_PERCENTAGE_SIG
struct SetBLDCSpeedPercentageEvent : public QP::QEvt {
	GPIO_PinState direction;
	uint8_t speed;
};

union LargestEvent {
	SetBLDCSpeedPercentageEvent a;
};

} // namespace BLDC

#endif /* INC_QPEVT_BLDCEVT_H_ */
