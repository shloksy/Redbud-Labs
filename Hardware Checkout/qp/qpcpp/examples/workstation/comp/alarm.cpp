//$file${.::alarm.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: comp.qm
// File:  ${.::alarm.cpp}
//
// This code has been generated by QM 5.2.4 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::alarm.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "alarm.hpp"
#include "clock.hpp"

Q_DEFINE_THIS_FILE

// Alarm component --------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.0.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Components::Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Components::Alarm} .......................................................

//${Components::Alarm::Alarm} ................................................
Alarm::Alarm()
 : QHsm(Q_STATE_CAST(&Alarm::initial))
{}

//${Components::Alarm::SM} ...................................................
Q_STATE_DEF(Alarm, initial) {
    //${Components::Alarm::SM::initial}
    m_alarm_time = 12U*60U;
    (void)e; // unused parameter

    QS_FUN_DICTIONARY(&Alarm::off);
    QS_FUN_DICTIONARY(&Alarm::on);

    return tran(&off);
}

//${Components::Alarm::SM::off} ..............................................
Q_STATE_DEF(Alarm, off) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::Alarm::SM::off}
        case Q_ENTRY_SIG: {
            // while in the off state, the alarm is kept in decimal format
            m_alarm_time = (m_alarm_time/60)*100 + m_alarm_time%60;
            BSP_showTime24H("*** Alarm OFF ", m_alarm_time, 100U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Alarm::SM::off}
        case Q_EXIT_SIG: {
            // upon exit, the alarm is converted to binary format
            m_alarm_time = (m_alarm_time/100U)*60U + m_alarm_time%100U;
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Alarm::SM::off::ALARM_ON}
        case ALARM_ON_SIG: {
            //${Components::Alarm::SM::off::ALARM_ON::[alarminrange?]}
            if ((m_alarm_time / 100U < 24U)
                && (m_alarm_time % 100U < 60U))
            {
                status_ = tran(&on);
            }
            //${Components::Alarm::SM::off::ALARM_ON::[else]}
            else {
                m_alarm_time = 0U;
                BSP_showTime24H("*** Alarm reset", m_alarm_time, 100U);
                status_ = Q_RET_HANDLED;
            }
            break;
        }
        //${Components::Alarm::SM::off::ALARM_SET}
        case ALARM_SET_SIG: {
            // while setting, the alarm is kept in decimal format
            m_alarm_time =
                 (10U * m_alarm_time + Q_EVT_CAST(SetEvt)->digit) % 10000U;
            BSP_showTime24H("*** Alarm reset ",  m_alarm_time, 100U);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${Components::Alarm::SM::on} ...............................................
Q_STATE_DEF(Alarm, on) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::Alarm::SM::on}
        case Q_ENTRY_SIG: {
            BSP_showTime24H("*** Alarm ON ",  m_alarm_time, 60U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Alarm::SM::on::ALARM_OFF}
        case ALARM_OFF_SIG: {
            status_ = tran(&off);
            break;
        }
        //${Components::Alarm::SM::on::ALARM_SET}
        case ALARM_SET_SIG: {
            BSP_showMsg("*** Cannot set Alarm when it is ON");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Alarm::SM::on::TIME}
        case TIME_SIG: {
            //${Components::Alarm::SM::on::TIME::[Q_EVT_CAST(TimeEvt)->current_ti~}
            if (Q_EVT_CAST(TimeEvt)->current_time == m_alarm_time) {
                BSP_showMsg("ALARM!!!");

                // asynchronously post the event to the container AO
                APP_alarmClock->POST(Q_NEW(QEvt, ALARM_SIG), this);
                status_ = Q_RET_HANDLED;
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//$enddef${Components::Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^