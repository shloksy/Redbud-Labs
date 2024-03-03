/**
 * @file MAX7317Evt.h
 *
 * @author awong
 *
 * Events for MAX7317 IO expander.
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_MAX7317EVT_H_
#define APP_COMMON_INCLUDE_QPEVT_MAX7317EVT_H_

#include "qpcpp.hpp"

#include <array>

namespace COMMON {
namespace MAX7317 {

// Important note!:
// This must be set in Project.h's LargestEvent for each width used.
template<size_t NumberOfDevicesDaisyChained>
struct SetOutputEvent : QP::QEvt {
    std::array<int, NumberOfDevicesDaisyChained> port;
    std::array<bool, NumberOfDevicesDaisyChained> enable;
};

} // namespace MAX7317
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_MAX7317EVT_H_ */
