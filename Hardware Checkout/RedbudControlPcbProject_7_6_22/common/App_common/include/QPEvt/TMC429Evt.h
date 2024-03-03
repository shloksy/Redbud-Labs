/**
 * @file TMC429Evt.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_TMC429EVT_H_
#define APP_COMMON_INCLUDE_QPEVT_TMC429EVT_H_

#include "qpcpp.hpp"

#include "TMC429Protocol.h"
#include "TMC429MgrPublic.h"

#include <stdint.h>

namespace COMMON {
namespace TMC429 {

enum ErrorCode {
    TMC429NoError,
    TMC429ErrorGeneric
};

struct SetMotorPositionEvent : QP::QEvt {
    int motor_index;

    uint32_t x_target;
};

struct SetMotorVelocityEvent : QP::QEvt {
    int motor_index;

    int16_t v_target;
};

struct ConfigMotorEvent : QP::QEvt {
    int motor_index;

    StepMotorConfig config;
};

struct RawCommandEvent : QP::QEvt {
    uint8_t addr;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
};

struct HomingCommandEvent : QP::QEvt {
    std::array<bool, NumStepperMotors> homing_enabled;

    std::array<int16_t, NumStepperMotors> homing_velocity;
};

struct QueryMotorFeedbackEvent : QP::QEvt {
    int motor_index;
};

struct MotorFeedbackEvent : QP::QEvt {
    ErrorCode error_code;

    int motor_index;

    StatusBits status;

    uint32_t position;
    int16_t velocity;
    uint8_t int_flags;
};

struct HomingCommandCompleteEvent : QP::QEvt {
    ErrorCode error_code;
};

struct SetModeEvent : QP::QEvt {
    int motor_index;

    MotionModes mode;
};

struct SetCurrentScalingEvent : QP::QEvt {
    StepperMotorIndex motor_index;
    IScalePercent is_agtat;
    IScalePercent is_aleat;
    IScalePercent is_v0;
    uint16_t a_threshold;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    SetMotorPositionEvent a;
    SetMotorVelocityEvent b;
    ConfigMotorEvent c;
    RawCommandEvent d;
    HomingCommandEvent e;
    QueryMotorFeedbackEvent g;
    MotorFeedbackEvent h;
    HomingCommandCompleteEvent i;
    SetModeEvent j;
    SetCurrentScalingEvent k;
};

} // namespace TMC429
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_TMC429EVT_H_ */
