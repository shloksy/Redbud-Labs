/**
 * @file AnalogMgrApp.cpp
 *
 * @author dsmoot
 */

#include "AnalogMgrApp.h"

namespace COMMON {
namespace ANALOG_INPUT {
const std::array<AdcLogicalIndexConfig, NumAdcPeripheralsUsed> AdcLogicalConfig
{
    AdcLogicalIndexConfig{Adc1FirstChannel, Adc1LastChannel, NumAdc1Channels}
};
} /*namespace ANALOG_INPUT */
} /*namespace COMMON */
