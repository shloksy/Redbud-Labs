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
//! @date Last updated on: 2022-12-13
//! @version Last updated for: @ref qpcpp_7_2_0
//!
//! @file
//! @brief QK/C++ sample port with all configurable options

#ifndef QK_PORT_HPP_
#define QK_PORT_HPP_

//lint -save -e1960    MISRA-C++:2008 Rule 7-3-1, Global declaration

//============================================================================
//! enable the context-switch callback
#define QF_ON_CONTEXT_SW   1

//============================================================================
// QK interrupt entry and exit

//! Define the ISR entry sequence, if the compiler supports writing
//! interrupts in C++.
//! @note This is just an example of #QK_ISR_ENTRY. You need to define
//! the macro appropriately for the CPU/compiler you're using. Also, some
//! QK ports will not define this macro, but instead will provide ISR
//! skeleton code in assembly.
#define QK_ISR_ENTRY() do { \
    ++QP::QF::intNest_;     \
} while (false)

//! Define the ISR exit sequence, if the compiler supports writing
//! interrupts in C++.
//! @note This is just an example of #QK_ISR_EXIT. You need to define
//! the macro appropriately for the CPU/compiler you're using. Also, some
//! QK ports will not define this macro, but instead will provide ISR
//! skeleton code in assembly.
#define QK_ISR_EXIT()     do {   \
    --QP::QF::intNest_;          \
    if (QP::QF::intNest_ == 0U) {\
        if (QK_sched_() != 0U) { \
            QK_activate_();      \
        }                        \
    }                            \
    else {                       \
        Q_ERROR();               \
    }                            \
} while (false)

extern "C" {

void FPU_save(void *ctx);     // defined in assembly
void FPU_restore(void *ctx);  // defined in assembly
extern void *impure_ptr;

} // extern "C"

//lint -restore

#include "qk.hpp" // QK platform-independent public interface

#endif // QK_PORT_HPP_
