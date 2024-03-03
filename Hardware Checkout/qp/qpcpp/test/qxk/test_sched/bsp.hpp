//============================================================================
// Product: BSP for system-testing QXK
// Last updated for version 7.2.0
// Last updated on  2022-12-14
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses/>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#ifndef BSP_HPP
#define BSP_HPP

namespace BSP {

void init(void);
void terminate(int16_t const result);

// for testing...
void trace(QP::QActive const *thr, char const *msg);
void wait4PB1(void);
void ledOn(void);
void ledOff(void);
void trigISR(void);

uint32_t romRead(int32_t offset, uint32_t fromEnd);
void romWrite(int32_t offset, uint32_t fromEnd, uint32_t value);

uint32_t ramRead(int32_t offset, uint32_t fromEnd);
void ramWrite(int32_t offset, uint32_t fromEnd, uint32_t value);

} // namespace BSP

enum TestSignals {
    TEST0_SIG = QP::Q_USER_SIG,
    TEST1_SIG,
    TEST2_SIG,
    TEST3_SIG,
    MAX_PUB_SIG,    // the last published signal

    MAX_SIG         // the last signal
};

#endif // BSP_HPP

