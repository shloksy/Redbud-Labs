/**
 * @file DigitalOutputApp.cpp
 *
 * @author awong
 */

#include "Project.h"

#include "DigitalOutputMgrApp.h"

#include "main.h"

#include <stdint.h>
#include <array>

// Must be defined before DigitalOutputMgr.h include.
Q_DEFINE_THIS_MODULE("DigitalOutputMgrApp")

#include "DigitalOutputMgr.h"

using namespace COMMON::DIGITAL_OUTPUT;

// Following is an application-defined set of pins that are accessible from
// DigitalOutputMgr.
// It is strongly recommended that any pins accessed here are only modified through this
// QP interface!
// Indices into this array must match DigitalOutPinIndex defined above.
static constexpr std::array<DOutPin, num_digital_outputs> AppDOutPinList = {
    DOutPin{OPTICAL_LED_1_GPIO_Port, OPTICAL_LED_1_Pin}, // OpticalLed1
    DOutPin{OPTICAL_LED_2_GPIO_Port, OPTICAL_LED_2_Pin}, // OpticalLed2
    DOutPin{OPTICAL_LED_3_GPIO_Port, OPTICAL_LED_3_Pin}, // OpticalLed3
    DOutPin{RPI_PWR_EN_GPIO_Port, RPI_PWR_EN_Pin}, // RPIPowerEnable
    DOutPin{HEATER1_GPIO_Port, HEATER1_Pin}, // Heater1
    DOutPin{HEATER2_GPIO_Port, HEATER2_Pin}, // Heater2
    DOutPin{PUMP_ON_GPIO_Port, PUMP_ON_Pin}  // PumpOn
};

struct DigitalOutputTraits {
    static constexpr enum_t DIGITAL_OUTPUT_SIG = SET_DIGITAL_OUTPUT_SIG;

    static constexpr std::array<DOutPin, num_digital_outputs> DOutPinList = AppDOutPinList;
};

static DigitalOutputMgr<DigitalOutputTraits> l_DigOutMgr;
QP::QActive * const AO_DigOutMgr = &l_DigOutMgr;
