/**
 * @file TMC429Config.h
 *
 * @author awong
 */

#ifndef INC_TMC429CONFIG_H_
#define INC_TMC429CONFIG_H_

#include "TMC429Protocol.h"

namespace COMMON {
namespace TMC429 {

static constexpr LSMD_NumberMotorDrivers NumStepperMotorDrivers = NumMotorDrivers1;

} // namespace TMC429
} // namespace COMMON

#endif /* INC_TMC429CONFIG_H_ */
