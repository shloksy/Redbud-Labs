# project_signals.py
# Contains all signals from Project.h.
#
# For usage with qtools scripts (qutest, qview, etc.).

# awong

from enum import Enum, auto
import project_common_signals as common

# This should stay up to date with UserSignals (Project.h):
class Signals(Enum):
    # I2CMgr
    I2C4_WRITE_TRANSACTION_SIG = common.Signals.LAST_COMMON_SIG.value
    I2C4_READ_TRANSACTION_SIG = auto()

    # SPIMgr
    SPI2_TRANSACTION_SIG = auto()

    # BLDCMgr
    SET_BLDC_SPEED_PERCENTAGE_SIG = auto()

    # AD7172_2Mgr
    AD7172_2_TRANSACTION_COMPLETE_SIG = auto()

    # SP160RegulatorMgr
    SP160_REGULATOR_TRANSACTION_COMPLETE_SIG = auto()

    # SP160PumpMgr
    SP160_PUMP_TRANSACTION_COMPLETE_SIG = auto()

    # HWHSC_SPIMgr
    HWHSC_SPI_TRANSACTION_COMPLETE_SIG = auto()
    HWHSC_SENSOR_DATA_SIG = auto()

    # MAX7317Mgr
    IOEXP_SET_OUTPUT_SIG = auto()

    # DigitalOutputMgr
    SET_DIGITAL_OUTPUT_SIG = auto()

    # MAX7315Mgr
    IOEXP2_I2C_TRANSACTION_COMPLETE_SIG = auto()
    IOEXP2_SET_OUTPUT_SIG = auto()

    MAX_PUB_SIG = auto()

# This should stay up to date with QSUserMessages (Project.h):
class QSUserMessages(Enum):
    LastUserMessage = common.QSUserMessages.LastCommonUserMessage.value
