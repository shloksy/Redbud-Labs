/**
 * @file ProjectCommon.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_PROJECT_COMMON_H_
#define APP_COMMON_INCLUDE_PROJECT_COMMON_H_

#include "qpcpp.hpp"

#include "QPEvt/TMC429Evt.h"
#include "AnalogMgrApp.h"

namespace COMMON {

enum CommonUserSignals {
    // TMC429Mgr
    TMC429_TRANSACTION_COMPLETE_SIG = QP::Q_USER_SIG,
    TMC429_CONFIG_MOTOR_SIG,
    TMC429_SET_MOTOR_POSITION_SIG,
    TMC429_SET_MOTOR_VELOCITY_SIG,
    TMC429_RAW_COMMAND_SIG,
    TMC429_HOMING_COMMAND_SIG,
    TMC429_SET_MODE_SIG,
    TMC429_HOMING_COMMAND_COMPLETE_SIG,
    TMC429_QUERY_MOTOR_FEEDBACK_SIG,
    TMC429_MOTOR_FEEDBACK_SIG,
    TMC429_SET_CURRENT_SCALING_SIG,

    // TouchMgr -> LVGLMgr
    LVGL_TOUCH_COORDINATES_SIG,

    //Analog input
    ADC_DATA_SIG,

    // TMC429DemoMgr
    QVIEW_MOTOR_HOME_SIG,

    LAST_COMMON_SIG // must be last in the enum list!
};

#ifdef Q_SPY
// TODO: Preferable for these to be application specific, but haven't done it yet.
enum CommonQSUserMessages {
    DisplayText = QP::QS_USER,
    SPIMgrMsg,
    I2CMgrMsg,
    TMC429MgrMsg,
    GT911MgrMsg,
    LVGLMgrMsg,
    DigitalInputMsg,
    DigitalOutputMsg,
    ADCMsg,
    SP160MgrMsg,
    HWHSC_SPIMgrMsg,
    TTPVentusMgrMsg,
    LastCommonUserMessage // must be last in the enum list!
};
#endif

template <typename T>
struct TypeEvt : public QP::QEvt {
    T data;
};

using BoolEvt = TypeEvt<bool>;
using Uint8Evt = TypeEvt<uint8_t>;
using Uint16Evt = TypeEvt<uint16_t>;
using Uint32Evt = TypeEvt<uint32_t>;
using FloatEvt = TypeEvt<float>;

#define MAX_TEXT_LEN 16
struct TextEvt : public QP::QEvt {
    char text[MAX_TEXT_LEN]; // text to deliver
};

// ADC_DATA_SIG
class AdcDataEvt : public QP::QEvt {
public:
    std::array<uint16_t, ANALOG_INPUT::NumAdcChannels> samples;
};

#ifdef Q_SPY
// Application specific QSpy IDs
enum QSAppIds {
    QSApp_DisplayText = QP::QS_AP_ID,
    QSApp_CmdReturn
};
#endif

union LargestEvent {
    BoolEvt a;
    Uint8Evt b;
    Uint16Evt c;
    Uint32Evt d;
    FloatEvt e;
    TextEvt f;
    TMC429::LargestEvent g;
    AdcDataEvt h;
};

} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_PROJECT_COMMON_H_ */
