/**
 * @file MAX7315Protocol.h
 *
 * @author awong
 *
 * I2C protocol header for MAX7315 IO expander.
 * See the MAX7315 datasheet for more information.
 */

#ifndef APP_COMMON_INCLUDE_MAX7315_MAX7315PROTOCOL_H_
#define APP_COMMON_INCLUDE_MAX7315_MAX7315PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <array>

namespace COMMON {
namespace MAX7315 {

// All I2C transactions to the MAX7315 are 2 bytes.
static constexpr size_t max7315_transaction_length = 2;
using max7315_trans_buf = std::array<uint8_t, max7315_transaction_length>; // Workable buffer for MAX7315.

static constexpr uint32_t ResetDelayTime_ms = 5; //! Arbitrary reset delay.

static constexpr int NumberOfPorts = 8; //! AKA number of I/O pins supported on this part.

enum RegisterAddress : uint8_t {
    ReadInputPortsRegister = 0x00, //! Register to read input ports.
    BlinkPhase0OutputsRegister = 0x01, //! Register to write blink phase 0 outputs.
    PortsConfigurationRegister = 0x03, //! Register to configure ports.
    BlinkPhase1OutputsRegister = 0x09, //! Register to write blink phase 1 outputs.
    MasterO8IntensityRegister = 0x0E, //! Register to configure master O8 intensity.
    ConfigurationRegister = 0x0F, //! Configuration register.
    OutputsIntensityP1P0 = 0x10, //! Phase 0 and phase 1 intensity.
    OutputsIntensityP3P2 = 0x11, //! Phase 2 and phase 3 intensity.
    OutputsIntensityP5P4 = 0x12, //! Phase 4 and phase 5 intensity.
    OutputsIntensityP7P6 = 0x13 //! Phase 6 and phase 7 intensity.
};

} // namespace MAX7315
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_MAX7315_MAX7315PROTOCOL_H_ */
