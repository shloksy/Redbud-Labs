
#ifndef INC_ANALOGAPP_H_
#define INC_ANALOGAPP_H_

#include <array>

namespace COMMON{
namespace ANALOG_INPUT {
/*
 * note the order of channels is set in the MX_ADC_Init function
 */
enum AdcPeripheralIndex {
    Adc1,

    NumAdcPeripheralsUsed // Must be last!
};

enum AdcLogicalChannelIndex {
    Adc1FirstChannel,
    Potentiometer1 = Adc1FirstChannel,
    Potentiometer2,
    Adc1LastChannel = Potentiometer2,

    NumAdcChannels
};

struct AdcLogicalIndexConfig {
    int FirstChannel;
    int LastChannel;
    int NumberOfChannels;
};

static constexpr int NumAdc1Channels = Adc1LastChannel - Adc1FirstChannel + 1;
static constexpr int NumAdc3Channels = 0;

extern const std::array<AdcLogicalIndexConfig, NumAdcPeripheralsUsed> AdcLogicalConfig;
} /*namespace ANALOG_INPUT */
} /*namespace COMMON */

#endif /* INC_ANALOGAPP_H_ */
