//! @file
//! @brief QEP/C++ port to Qt
//! @cond
//============================================================================
//! Last updated for version 6.8.0 / Qt 5.x
//! Last updated on  2020-01-13
//!
//!                    Q u a n t u m  L e a P s
//!                    ------------------------
//!                    Modern Embedded Software
//!
//! Copyright (C) 2005-2019 Quantum Leaps. All rights reserved.
//!
//! This program is open source software: you can redistribute it and/or
//! modify it under the terms of the GNU General Public License as published
//! by the Free Software Foundation, either version 3 of the License, or
//! (at your option) any later version.
//!
//! Alternatively, this program may be distributed and modified under the
//! terms of Quantum Leaps commercial licenses, which expressly supersede
//! the GNU General Public License and are specifically designed for
//! licensees interested in retaining the proprietary status of their code.
//!
//! This program is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//! GNU General Public License for more details.
//!
//! You should have received a copy of the GNU General Public License
//! along with this program. If not, see <www.gnu.org/licenses>.
//!
//! Contact information:
//! <www.state-machine.com/licensing>
//! <info@state-machine.com>
//============================================================================
//! @endcond

#ifndef QEP_PORT_HPP
#define QEP_PORT_HPP

// provide QEvt constructors
#define Q_EVT_CTOR    1

// provide QEvt virtual destructor
#define Q_EVT_VIRTUAL 1

#include <cstdint>    // Exact-width types. C++11 dtandard
#include "qep.hpp"    // QEP platform-independent public interface

#endif // QEP_PORT_HPP

