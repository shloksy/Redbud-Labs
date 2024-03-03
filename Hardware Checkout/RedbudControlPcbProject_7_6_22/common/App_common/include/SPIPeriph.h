/**
 * @file SPIPeriph.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_SPIPERIPH_H_
#define APP_COMMON_INCLUDE_SPIPERIPH_H_

#include <stdint.h>

namespace COMMON {
namespace SPI {

enum class ClockPolarity : uint32_t {
    Low,
    High
};

enum class ClockPhase : uint32_t {
    OneEdge,
    TwoEdge
};

struct SpiConfig {
    uint32_t setup_time_us; //! Delay between when nCS is asserted and when the first data byte is sent.
    uint32_t hold_time_us; //! Delay between when the last data byte is sent and when nCS is deasserted.

    ClockPolarity clock_polarity;
    ClockPhase clock_phase;
};

} // namespace SPI
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_SPIPERIPH_H_ */
