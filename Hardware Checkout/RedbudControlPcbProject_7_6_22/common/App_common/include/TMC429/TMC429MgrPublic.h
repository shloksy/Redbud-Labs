/**
 * @file TMC429MgrPublic.h
 *
 * @author awong
 */

#ifndef INCLUDE_TMC429MGRPUBLIC_H_
#define INCLUDE_TMC429MGRPUBLIC_H_

#include "TMC429Protocol.h"

namespace COMMON {
namespace TMC429 {

// TODO: Only added here for testing using QView.  These kind of details probably don't belong here, but will
// be updated when the public interface to TMC429.h becomes more manageable.
// Configuration defined in TMC429 datasheet section 12 steps 3 through 9.
struct StepMotorConfig {
    uint16_t velocity_min; // 11-bits unsigned.
    uint16_t velocity_max; // 11-bits unsigned.

    uint16_t acceleration_max; // 11 bit unsigned.

    // Ordered this way so that we can have better alignment:
    uint8_t pulse_div; // 4 bit.
    uint8_t ramp_div; // 4 bit.
    MicrostepResolutionSelection ustep_resolution_selection; // "USRS", 3 bits (some conflicts in documentation that states 2-bits).

    MotionModes ramp_mode; // 2 bit.

    // TODO: REF_CONF?
};

} // namespace TMC429
} // namespace COMMON

#endif /* INCLUDE_TMC429MGRPUBLIC_H_ */
