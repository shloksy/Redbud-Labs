/**
 * @file PMulPDiv.h
 *
 * @author awong
 *
 * Helper class that calculates PMUL and PDIV constants from constants a_max, ramp_div, and
 * pulse_div.
 */

#ifndef APP_COMMON_SOURCE_TMC429_PMULPDIV_H_
#define APP_COMMON_SOURCE_TMC429_PMULPDIV_H_

#include <stdint.h>

namespace COMMON::TMC429 {

class PMulPDiv {
private:
    struct input {
        uint32_t a_max;
        int32_t ramp_div;
        int32_t pulse_div;
    };

    struct output {
        int32_t p_mul;
        int32_t p_div;
    };

    input input_params;
    output outputs_calculated;

public:
    PMulPDiv() :
        input_params{},
        outputs_calculated{}
        {
            ;
        }

        void calculate(uint32_t a_max, int32_t ramp_div, int32_t pulse_div) {
            input_params = {a_max, ramp_div, pulse_div};
            calculate(a_max);
        }

        void calculate(uint32_t a_max);

        int get_p_mul() {
            return outputs_calculated.p_mul;
        }
        int get_p_div() {
            return outputs_calculated.p_div;
        }
};

} // namespace COMMON::TMC429

#endif /* APP_COMMON_SOURCE_TMC429_PMULPDIV_H_ */
