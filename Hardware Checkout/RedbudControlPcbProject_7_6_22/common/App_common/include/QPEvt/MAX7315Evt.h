/**
 * @file MAX7315Evt.h
 *
 * @author awong
 *
 * Events for MAX7315 IO expander.
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_MAX7315EVT_H_
#define APP_COMMON_INCLUDE_QPEVT_MAX7315EVT_H_

#include "qpcpp.hpp"

#include <array>

namespace COMMON {
namespace MAX7315 {

struct SetOutputEvent : QP::QEvt {
    int port_index;
    bool enable;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    SetOutputEvent a;
};

} // namespace MAX7315
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_MAX7315EVT_H_ */
