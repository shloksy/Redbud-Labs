/**
 * @file QP_AppInit.cpp
 *
 * @author awong
 *
 * Adapted from one of jcochran's implementations.
 */

#include "Config.h"

#include "Project.h"

#include "AOHelpers.h"

#include "qpcpp.hpp"
#include "bsp.h"

#include "adc.h"
#include "AnalogMgr.h"

#include "DigitalInputMgrApp.h"
#include "DigitalOutputMgrApp.h"
#include "BLDCMgr.h"

using namespace QP;

Q_DEFINE_THIS_MODULE("QP_AppInit")

static void RegisterProjectSignals() {
    // COMMON signals:
    QS_SIG_DICTIONARY(COMMON::TMC429_TRANSACTION_COMPLETE_SIG, nullptr);
    QS_SIG_DICTIONARY(COMMON::TMC429_CONFIG_MOTOR_SIG, nullptr);
    QS_SIG_DICTIONARY(COMMON::TMC429_SET_MOTOR_POSITION_SIG, nullptr);
    QS_SIG_DICTIONARY(COMMON::TMC429_SET_MOTOR_VELOCITY_SIG, nullptr);
    QS_SIG_DICTIONARY(COMMON::TMC429_RAW_COMMAND_SIG, nullptr);

    // Project signals:
    QS_SIG_DICTIONARY(I2C4_WRITE_TRANSACTION_SIG, nullptr);
    QS_SIG_DICTIONARY(I2C4_READ_TRANSACTION_SIG, nullptr);

    QS_SIG_DICTIONARY(SPI2_TRANSACTION_SIG, nullptr);

    QS_SIG_DICTIONARY(COMMON::ADC_DATA_SIG, nullptr);
    QS_SIG_DICTIONARY(SET_BLDC_SPEED_PERCENTAGE_SIG, nullptr);
    QS_SIG_DICTIONARY(AD7172_2_TRANSACTION_COMPLETE_SIG, nullptr);
    QS_SIG_DICTIONARY(SP160_REGULATOR_TRANSACTION_COMPLETE_SIG, nullptr);
    QS_SIG_DICTIONARY(SP160_PUMP_TRANSACTION_COMPLETE_SIG, nullptr);
    QS_SIG_DICTIONARY(HWHSC_SPI_TRANSACTION_COMPLETE_SIG, nullptr);
    QS_SIG_DICTIONARY(HWHSC_SENSOR_DATA_SIG, nullptr);
    QS_SIG_DICTIONARY(SET_DIGITAL_OUTPUT_SIG, nullptr);
    QS_SIG_DICTIONARY(IOEXP2_SET_OUTPUT_SIG, nullptr);

    // COMMON user messages:
    QS_USR_DICTIONARY(COMMON::DisplayText);
    QS_USR_DICTIONARY(COMMON::SPIMgrMsg);
    QS_USR_DICTIONARY(COMMON::I2CMgrMsg);
    QS_USR_DICTIONARY(COMMON::TMC429MgrMsg);
    QS_USR_DICTIONARY(COMMON::GT911MgrMsg);
    QS_USR_DICTIONARY(COMMON::LVGLMgrMsg);
    QS_USR_DICTIONARY(COMMON::DigitalInputMsg);
    QS_USR_DICTIONARY(COMMON::DigitalOutputMsg);
    QS_USR_DICTIONARY(COMMON::ADCMsg);
    QS_USR_DICTIONARY(COMMON::SP160MgrMsg);
    QS_USR_DICTIONARY(COMMON::HWHSC_SPIMgrMsg);
    QS_USR_DICTIONARY(COMMON::TTPVentusMgrMsg);

    // Project user messages:
    // None.

    // Active objects:
    QS_OBJ_DICTIONARY(COMMON::AO_Blinky);
    QS_OBJ_DICTIONARY(AO_SPI2Mgr);
    QS_OBJ_DICTIONARY(AO_I2CAMgr);
    QS_OBJ_DICTIONARY(AO_TMC429Mgr);
    QS_OBJ_DICTIONARY(AO_DigInMgr);
    QS_OBJ_DICTIONARY(AO_DigOutMgr);
    QS_OBJ_DICTIONARY(AO_AnlgMgr);
    QS_OBJ_DICTIONARY(AO_BLDCMgr);
    QS_OBJ_DICTIONARY(AO_AD7172_2Mgr);
    QS_OBJ_DICTIONARY(AO_IOExpMgr);
    QS_OBJ_DICTIONARY(AO_SP160RegulatorMgr);
    QS_OBJ_DICTIONARY(AO_SP160PumpMgr);
    QS_OBJ_DICTIONARY(AO_HWHSC_SPIMgr);
    QS_OBJ_DICTIONARY(AO_TTPVentusMgr);
    QS_OBJ_DICTIONARY(AO_IOExpMgr2);
}

void QP_AppInit(int argv, const char* argc[]) {
    (void) argv;
    (void) argc;

    static QSubscrList subscrSto[MAX_PUB_SIG];
    static QF_MPOOL_EL(LargestEvent) smlPoolSto[50];

    static_cast<COMMON::ANALOG_INPUT::AnalogMgr *>(AO_AnlgMgr)->SetpAdcHandle(COMMON::ANALOG_INPUT::AdcPeripheralIndex::Adc1, &hadc1);

    QF::init(); // initialize the framework and the underlying RT kernel
    Q_ALLEGE(QS_INIT(nullptr));
    //  QS_FILTER_ON(QS_UA_RECORDS);

    RegisterProjectSignals();

#ifndef Q_UTEST
    QS_FILTER_ON(COMMON::DisplayText);
#endif

    //ok to do this after QF init, this really does nothing on hardware as of this writing.
    COMMON::BSP::init(); // initialize the Board Support Package

    QF::psInit(subscrSto, Q_DIM(subscrSto));
    QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

#ifndef Q_UTEST
//    QS_FILTER_ON(COMMON::TMC429MgrMsg);
//    QS_FILTER_ON(COMMON::I2CMgrMsg);
//    QS_FILTER_ON(COMMON::SPIMgrMsg);
//    QS_FILTER_ON(COMMON::SP160MgrMsg);
//    QS_FILTER_ON(COMMON::HWHSC_SPIMgrMsg);
//    QS_FILTER_ON(COMMON::TTPVentusMgrMsg);
#endif

    // instantiate and start the active objects...
    static COMMON::AOStart<20> startAos[] {
            COMMON::AO_Blinky,
            AO_TMC429DemoMgr,
            AO_BLDCMgr,
            AO_AnlgMgr,
            AO_TMC429Mgr,
            AO_IOExpMgr2,
            AO_IOExpMgr,
            AO_TTPVentusMgr,
            AO_HWHSC_SPIMgr,
            AO_SP160RegulatorMgr,
            AO_SP160PumpMgr,
            AO_AD7172_2Mgr,
            AO_DigInMgr,
            AO_SPI2Mgr,
            AO_I2CAMgr,
            AO_DigOutMgr,
    };
    (void) startAos;
}

