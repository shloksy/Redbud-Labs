/**
 * @file LVGLEvt.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_QPEVT_LVGLEVT_H_
#define APP_COMMON_INCLUDE_QPEVT_LVGLEVT_H_

#include "qpcpp.hpp"

#include <array>
#include <cstddef>

namespace COMMON {
namespace LVGL {

struct TouchCoordinatesEvent : QP::QEvt {
    bool touch_detected;

    size_t x;
    size_t y;

    size_t size;
};

//! Contains a union which helps application determine largest event in this module.
union LargestEvent {
    TouchCoordinatesEvent a;
};

} // namespace LVGL
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_QPEVT_LVGLEVT_H_ */
