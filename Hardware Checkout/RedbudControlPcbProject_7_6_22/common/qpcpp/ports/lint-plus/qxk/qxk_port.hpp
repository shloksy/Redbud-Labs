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
//! @date Last updated on: 2021-12-23
//! @version Last updated for: @ref qpcpp_7_0_0
//!
//! @file
//! @brief QXK/C++ port example, Generic C++ compiler
//! @description
//! This is an example QP/C++ port with the documentation for the main
//! items, such as configuration macros, functions, and includes.

#ifndef QXK_PORT_HPP
#define QXK_PORT_HPP

//lint -save -e1960    MISRA-C++:2008 Rule 7-3-1, Global declaration

//============================================================================
//! determination if the code executes in the ISR context
//! (used internally in QXK only)
#define QXK_ISR_CONTEXT_() (getSR() != 0U)

//! trigger context switch (used internally in QXK only)
#define QXK_CONTEXT_SWITCH_() (trigSWI())

//============================================================================
//! activate the context-switch callback
#define QXK_ON_CONTEXT_SW 1

//============================================================================
// QXK interrupt entry and exit

//! Define the ISR entry sequence, if the compiler supports writing
//! interrupts in C++.
//! @note This is just an example of #QK_ISR_ENTRY. You need to define
//! the macro appropriately for the CPU/compiler you're using. Also, some
//! QK ports will not define this macro, but instead will provide ISR
//! skeleton code in assembly.
#define QXK_ISR_ENTRY() do { \
    ++QXK_attr_.intNest;     \
} while (false)


//! Define the ISR exit sequence, if the compiler supports writing
//! interrupts in C++.
//! @note This is just an example of #QK_ISR_EXIT. You need to define
//! the macro appropriately for the CPU/compiler you're using. Also, some
//! QK ports will not define this macro, but instead will provide ISR
//! skeleton code in assembly.
#define QXK_ISR_EXIT() do {        \
    --QXK_attr_.intNest;           \
    if (QXK_attr_.intNest == 0U) { \
        if (QXK_sched_() != 0U) {  \
            QXK_activate_();       \
        }                          \
    }                              \
    else {                         \
        Q_ERROR();                 \
    }                              \
} while (false)

extern "C" {

std::uint32_t getSR(void);
void trigSWI(void);

} // extern "C"

//lint -restore

#include "qxk.hpp" // QXK platform-independent public interface

#endif // QXK_PORT_HPP
