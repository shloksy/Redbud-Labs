/**
 * HWHSC_TempSensor.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_HWHSC_HWHSC_TEMPSENSOR_H_
#define APP_COMMON_INCLUDE_HWHSC_HWHSC_TEMPSENSOR_H_

#include <stdint.h>

namespace COMMON {
namespace HWHSC_SPI {
namespace TempSensor {

// Used if full 11-bit temperature is read (Data Byte 3 and 4).
inline float calc_11(uint16_t temp_data) {
    return ((static_cast<float>(temp_data) / 2047.0f) * 200.0f) - 50.0f;
}

// Used if only obtaining temperature bits 10 through 3 (Data Byte 3).
inline float calc_8(uint8_t temp_data) {
    return calc_11(static_cast<uint16_t>(temp_data) << 3);
}

} // TempSensor

// TODO: Pressure sensor calculation!

} // HWHSC_SPI
} // COMMON

#endif /* APP_COMMON_INCLUDE_HWHSC_HWHSC_TEMPSENSOR_H_ */
