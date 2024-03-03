/**
 * @file qphelper.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_QPHELPER_H_
#define APP_COMMON_INCLUDE_QPHELPER_H_

#include "qpcpp.hpp"

//! Macro to generate a definition of a state-handler for a given state
//! in a subclass of QP::QHsm.
#define Q_STATE_DEF_INTERNAL(subclass_, state_) \
    static QP::QState state_(void * const me, QP::QEvt const * const e) {\
        return static_cast<subclass_ *>(me)->state_ ## _h(e); } \
    QP::QState state_ ## _h(QP::QEvt const * const e)

#endif /* APP_COMMON_INCLUDE_QPHELPER_H_ */
