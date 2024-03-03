/**
 * @file DigitalOutputEvt.h
 *
 * @author awong
 *
 * Events for the DigitalOutputMgr AO.
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_DIGITALOUTPUTEVT_H_
#define APP_COMMON_INCLUDE_QPEVT_DIGITALOUTPUTEVT_H_

#include "qpcpp.hpp"

namespace COMMON {
namespace DIGITAL_OUTPUT {

//! Event that enables/disables a particular digital output index.
//! Corresponds with application defined DIGITAL_OUTPUT_SIG in DigitalOutput traits.
struct DigitalOutputEvent : QP::QEvt {
    //! Digital output index that corresponds with user defined pin list (from DigitalOutputMgrApp.h).
    size_t digital_output_index;
    //! Whether the output pin should be enabled or disabled.
    bool enable;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    DigitalOutputEvent a; //! a
};

} // namespace COMMON
} // namespace DIGITAL_OUTPUT

#endif /* APP_COMMON_INCLUDE_QPEVT_DIGITALOUTPUTEVT_H_ */
