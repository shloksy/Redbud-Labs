/**
 * @file DigitalInputApp.cpp
 *
 * @author dsmoot
 * @author awong
 */

#include "Project.h"
#include "qpcpp.hpp"

#include "main.h"

#include "DigitalInputMgrApp.h"

#include <stdint.h>
#include <array>

// Must be defined before DigitalInputMgr.h include.
Q_DEFINE_THIS_MODULE("DigitalInputMgrApp")

#include "DigitalInputMgr.h"

using namespace COMMON::DIGITAL_INPUT;

// Following is an application-defined set of pins that are accessible from
// DigitalInputMgr.
// It is strongly recommended that any pins accessed here are only modified through this
// QP interface!
// Indices into this array must match DigitalInPinIndex defined above.
static constexpr std::array<DInPin, num_polled_inputs> AppDInPinList = {
    DInPin{PUSHBUTTON_SWITCH_GPIO_Port, PUSHBUTTON_SWITCH_Pin, 0, 4}, //! PushButtonSwitch
    DInPin{PUMP_FEEDBACK_GPIO_Port, PUMP_FEEDBACK_Pin, 0, 4}, //! PumpFeedback
    DInPin{ROCKER_SWITCH1_GPIO_Port, ROCKER_SWITCH1_Pin, 0, 4}, //! RockerSwitch1
    DInPin{FAN_SPEED_FEEDBACK_GPIO_Port, FAN_SPEED_FEEDBACK_Pin, 0, 4}, //! FanSpeedFeedback
    DInPin{MOTION_HALL_FEEDBACK_GPIO_Port, MOTION_HALL_FEEDBACK_Pin, 0, 4}, //! MotionHallFeedback
    DInPin{MOTION_FB_A_GPIO_Port, MOTION_FB_A_Pin, 0, 4}, //! MotionFeedbackA
    DInPin{MOTION_FB_B_GPIO_Port, MOTION_FB_B_Pin, 0, 4}, //! MotionFeedbackB
    DInPin{MOTION_FB_C_GPIO_Port, MOTION_FB_C_Pin, 0, 4}, //! MotionFeedbackC
    DInPin{RPI_PWR_nFLG_GPIO_Port, RPI_PWR_nFLG_Pin, 0, 4}, //! RPIPwrNFlag
    DInPin{OPTICAL_FEEDBACK1_GPIO_Port, OPTICAL_FEEDBACK1_Pin, 0, 4}, //! OpticalFeedback1
    DInPin{OPTICAL_FEEDBACK2_GPIO_Port, OPTICAL_FEEDBACK2_Pin, 0, 4}, //! OpticalFeedback2
    DInPin{OPTICAL_FEEDBACK3_GPIO_Port, OPTICAL_FEEDBACK3_Pin, 0, 4}, //! OpticalFeedback3
    DInPin{OPTICAL_FEEDBACK4_GPIO_Port, OPTICAL_FEEDBACK4_Pin, 0, 4} //! OpticalFeedback4
};

struct DigitalInputTraits {
    //! Application defined pin list (see definition above).
    static constexpr std::array<DInPin, num_polled_inputs> DInPinList = AppDInPinList;

    //! Poll period for AO to check all input pins.
    static constexpr uint32_t PollPeriodMs = 200;
};

static DigitalInputMgr<DigitalInputTraits> l_DigInMgr;
QP::QActive * const AO_DigInMgr = &l_DigInMgr;
