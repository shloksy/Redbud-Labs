/**
 * @file PMulPDiv.cpp
 *
 * @author awong
 *
 * Helper class that calculates PMUL and PDIV constants.  This was pretty much
 * pulled directly from TMC429 eval example code.
 * Relevant sections in the TMC429 documentation are: 8.1.7 and 8.1.10.
 */

#include <stdint.h>

#include "PMulPDiv.h"

namespace COMMON::TMC429 {

void PMulPDiv::calculate(uint32_t a_max) {
    const auto ramp_div = input_params.ramp_div;
    const auto pulse_div = input_params.pulse_div;
    input_params.a_max = a_max;

    int32_t pm = -1;
    int32_t pd = -1;
    float p, p_reduced;

    // -1 indicates : no valid pair found
    pm = -1;
    pd = -1;

    // Translation from code: Exponent positive or 0.
    if (ramp_div >= pulse_div) {
        p = a_max / ( 128.0 * (1 << (ramp_div - pulse_div)));
    }
    // Translation from code: Exponent negative.
    else {
        p = a_max / ( 128.0 / (1 << (pulse_div - ramp_div)));
    }

    // TODO: Magic number 0.988???
    p_reduced = p * 0.988;

    for (int pdiv = 0; pdiv <= 13; pdiv++)
    {
        int pmul = static_cast<int>((p_reduced * 8.0 * (1 << pdiv)) - 128);

        if ( (0 <= pmul) && (pmul <= 127) )
        {
            pm = pmul + 128;
            pd = pdiv;
        }
    }

    outputs_calculated.p_mul = pm;
    outputs_calculated.p_div = pd;
}

} // namespace COMMON::TMC429
