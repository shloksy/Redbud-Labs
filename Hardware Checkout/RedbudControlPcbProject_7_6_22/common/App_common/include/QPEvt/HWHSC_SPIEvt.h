/**
 * @file HWHSC_SPIEvt.h
 *
 * @author awong
 *
 * Events for the HWHSC_SPIMgr AO.
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_HWHSC_SPIEVT_H_
#define APP_COMMON_INCLUDE_QPEVT_HWHSC_SPIEVT_H_

#include "qpcpp.hpp"

#include "HWHSC/HWHSC_SPIProtocol.h"

namespace COMMON {
namespace HWHSC_SPI {

//! Event that reports status and sensor data from the Honeywell pressure sensor.
//! Corresponds with application defined SENSOR_DATA_SIG in HWHSC traits.
struct SensorEvent : QP::QEvt {
    //! Contains status of the Honeywell pressure sensor as well as pressure and temperature data.
    SensorData data;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    SensorEvent a; //! a
};

} // namespace HWHSC_SPI
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_HWHSC_SPIEVT_H_ */
