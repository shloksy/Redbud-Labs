//============================================================================
// QP/C++ Real-Time Embedded Framework (RTEF)
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com>
// <info@state-machine.com>
//============================================================================
//! @date Last updated on: 2022-11-11
//! @version Last updated for: @ref qpcpp_7_1_3
//!
//! @file
//! @brief QF/C++ to MSP40, QUTEST unit test harness, generic C99 compiler

#ifndef QF_PORT_HPP
#define QF_PORT_HPP

// QUTEST event queue and thread types
#define QF_EQUEUE_TYPE QEQueue
//#define QF_OS_OBJECT_TYPE
//#define QF_THREAD_TYPE

// The maximum number of active objects in the application, see NOTE01
#define QF_MAX_ACTIVE        16U

#define QF_EVENT_SIZ_SIZE    1U
#define QF_EQUEUE_CTR_SIZE   1U
#define QF_MPOOL_SIZ_SIZE    1U
#define QF_MPOOL_CTR_SIZE    1U
#define QF_TIMEEVT_CTR_SIZE  2U

// QF interrupt disable/enable
#define QF_INT_DISABLE()     (++QP::QF::intLock_)
#define QF_INT_ENABLE()      (--QP::QF::intLock_)

// QF critical section
// QF_CRIT_STAT_TYPE not defined
#define QF_CRIT_ENTRY(dummy) QF_INT_DISABLE()
#define QF_CRIT_EXIT(dummy)  QF_INT_ENABLE()

// QF_LOG2 not defined -- use the internal LOG2() implementation

#include "qep_port.hpp"  // QEP port
#include "qequeue.hpp"   // QUTEST port uses QEQueue event-queue
#include "qmpool.hpp"    // QUTEST port uses QMPool memory-pool
#include "qf.hpp"        // QF platform-independent public interface

//============================================================================
// interface used only inside QF, but not in applications

#ifdef QP_IMPL

    // QUTEST scheduler locking (not used)
    #define QF_SCHED_STAT_
    #define QF_SCHED_LOCK_(dummy) ((void)0)
    #define QF_SCHED_UNLOCK_()    ((void)0)

    // native event queue operations
    #define QACTIVE_EQUEUE_WAIT_(me_) \
        Q_ASSERT_ID(110, (me_)->m_eQueue.m_frontEvt != nullptr)
    #define QACTIVE_EQUEUE_SIGNAL_(me_) \
        (QF::readySet_.insert(   \
            static_cast<std::uint_fast8_t>((me_)->m_prio)))

    // native QF event pool operations
    #define QF_EPOOL_TYPE_  QMPool
    #define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
        (p_).init((poolSto_), (poolSize_), (evtSize_))
    #define QF_EPOOL_EVENT_SIZE_(p_)  ((p_).getBlockSize())
    #define QF_EPOOL_GET_(p_, e_, m_, qs_id_) \
        ((e_) = static_cast<QEvt *>((p_).get((m_), (qs_id_))))
    #define QF_EPOOL_PUT_(p_, e_, qs_id_) ((p_).put((e_), (qs_id_)))

    #include "qf_pkg.hpp" // internal QF interface

#endif // QP_IMPL

//============================================================================
// NOTE01:
// The maximum number of active objects QF_MAX_ACTIVE can be increased
// up to 64, if necessary. Here it is set to a lower level to save some RAM.
//

#endif // QF_PORT_HPP
