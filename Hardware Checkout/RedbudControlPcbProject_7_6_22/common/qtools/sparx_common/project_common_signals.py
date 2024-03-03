# project_common_signals.py
# Contains all signals from Project_Common.h.
#
# For usage with qtools scripts (qutest, qview, etc.).

# awong

from enum import Enum, auto

# QP constant.  Starting signal for application user signals.
_QP_Q_USER_SIG = 4

# This should stay up to date with COMMON::CommonUserSignals (Project_Common.h):
class Signals(Enum):
    # TMC429Mgr
    TMC429_TRANSACTION_COMPLETE_SIG = _QP_Q_USER_SIG
    TMC429_CONFIG_MOTOR_SIG = auto()
    TMC429_SET_MOTOR_POSITION_SIG = auto()
    TMC429_SET_MOTOR_VELOCITY_SIG = auto()
    TMC429_RAW_COMMAND_SIG = auto()
    TMC429_HOMING_COMMAND_SIG = auto()
    TMC429_SET_MODE_SIG = auto()
    TMC429_HOMING_COMMAND_COMPLETE_SIG = auto()
    TMC429_QUERY_MOTOR_FEEDBACK_SIG = auto()
    TMC429_MOTOR_FEEDBACK_SIG = auto()
    TMC429_SET_CURRENT_SCALING_SIG = auto()

    # TouchMgr -> LVGLMgr
    LVGL_TOUCH_COORDINATES_SIG = auto()

    # Analog input
    ADC_DATA_SIG = auto()

    # TMC429DemoMgr
    QVIEW_MOTOR_HOME_SIG = auto()
    
    # insert more common signals above here
    LAST_COMMON_SIG = auto() # must be last in the enum list!

# QP constant.  Starting signal for QSpy user signals.
_QP_QS_USER = 100

# This should stay up to date with COMMON::CommonQSUserMessages (Project_Common.h):
class QSUserMessages(Enum):
    DisplayText = _QP_QS_USER
    SPIMgrMsg = auto()
    I2CMgrMsg = auto()
    TMC429MgrMsg = auto()
    GT911MgrMsg = auto()
    LVGLMgrMsg = auto()
    DigitalInputMsg = auto()
    DigitalOutputMsg = auto()
    ADCMsg = auto()
    SP160MgrMsg = auto()
    HWHSC_SPIMgrMsg = auto()
    TTPVentusMgrMsg = auto()
    
    # insert more common signals above here
    LastCommonUserMessage = auto() # must be last in the enum list!