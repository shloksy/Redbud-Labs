/**
 * @file Project.h
 *
 * @author awong
 */

#ifndef APP_INCLUDE_PROJECT_H_
#define APP_INCLUDE_PROJECT_H_

#include "Config.h"

#include "qpcpp.hpp"
#include "qphelper.h"

#include "QPEvt/LVGLEvt.h"
#include "QPEvt/BLDCEvt.h"
#include "QPEvt/HWHSC_SPIEvt.h"
#include "QPEvt/MAX7317Evt.h"
#include "QPEvt/DigitalOutputEvt.h"
#include "QPEvt/MAX7315Evt.h"

constexpr uint32_t AdcSamplePeriodMs = 500;

// TODO: Hack to get around UART conflict with LCD screen.
#define COREH743I_C_PROJECT 1

// TODO: Perhaps union of all I2C depths expected from I2C devices on the bus.  Hard-coded for now.
// TODO: Also could be I2C bus specific.  This would require templating the events with hard-coded
//       lengths.
static constexpr size_t AppMaxI2CTransactionLength = 8;
// MaxI2CTransactionLength must be defined in the Project.h before I2CEvt.h.
#include "QPEvt/I2CEvt.h"

// TODO: Perhaps union of all SPI depths expected from SPI devices on the bus.  Hard-coded for now.
// TODO: Also could be SPI bus specific.  This would require templating the events with hard-coded
//       lengths.
static constexpr size_t AppMaxSPITransactionLength = 20;
// MaxSPITransactionLength must be defined in the Project.h before SPIEvt.h.
#include "QPEvt/SPIEvt.h"

#include "Project_Common.h"

enum CM7UserSignals {
    // I2CMgr
    I2C4_WRITE_TRANSACTION_SIG = COMMON::LAST_COMMON_SIG,
    I2C4_READ_TRANSACTION_SIG,

    // SPIMgr
    SPI2_TRANSACTION_SIG,

    // BLDCMgr
    SET_BLDC_SPEED_PERCENTAGE_SIG,

    // AD7172_2Mgr
    AD7172_2_TRANSACTION_COMPLETE_SIG,

    // SP160RegulatorMgr
    SP160_REGULATOR_TRANSACTION_COMPLETE_SIG,

    // SP160PumpMgr
    SP160_PUMP_TRANSACTION_COMPLETE_SIG,

    // HWHSC_SPIMgr
    HWHSC_SPI_TRANSACTION_COMPLETE_SIG,
    HWHSC_SENSOR_DATA_SIG,

    // MAX7317Mgr
    IOEXP_SET_OUTPUT_SIG,

    // DigitalOutputMgr
    SET_DIGITAL_OUTPUT_SIG,

    // MAX7315Mgr
    IOEXP2_I2C_TRANSACTION_COMPLETE_SIG,
    IOEXP2_SET_OUTPUT_SIG,

    MAX_PUB_SIG
};

#ifdef Q_SPY
enum CM7QSUserMessages {
    DummyMsg = COMMON::LastCommonUserMessage
};
#endif

//! Number of MAX7317 devices that are daisy chained over SPI.  This is used to size the
//! QP event array.
static constexpr size_t NumberOfMax7317DaisyChained = 2;

//! NOTE:  If you add an event, add it to the list below.
//! This will help ensure that the event pool block size is large
//! enough to store all of the events.
//! Project defined union to help application determine largest QP event that exists.  This is used
//! to size the QP free event heap.
union LargestEvent {
    COMMON::LargestEvent a;
    COMMON::I2C::LargestEvent b;
    COMMON::SPI::LargestEvent c;
    BLDC::LargestEvent d;
    COMMON::HWHSC_SPI::LargestEvent e;
    COMMON::MAX7317::SetOutputEvent<NumberOfMax7317DaisyChained> f;
    COMMON::MAX7315::LargestEvent g;
};

extern QP::QActive * const AO_TMC429Mgr;
extern QP::QActive * const AO_TMC429DemoMgr;
extern QP::QActive * const AO_SPI2Mgr;
extern QP::QActive * const AO_I2CAMgr;
extern QP::QActive * const AO_DigInMgr;
extern QP::QActive * const AO_DigOutMgr;
extern QP::QActive * const AO_AnlgMgr;
extern QP::QActive * const AO_AD7172_2Mgr;
extern QP::QActive * const AO_BLDCMgr;
extern QP::QActive * const AO_SP160RegulatorMgr;
extern QP::QActive * const AO_SP160PumpMgr;
extern QP::QActive * const AO_HWHSC_SPIMgr;
extern QP::QActive * const AO_TTPVentusMgr;
extern QP::QActive * const AO_IOExpMgr;
extern QP::QActive * const AO_IOExpMgr2;

namespace COMMON {
extern QP::QActive * const AO_Blinky;
} // namespace COMMON

#endif /* APP_INCLUDE_PROJECT_H_ */
