//$file${src::qhsmtst.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qhsmtst.qm
// File:  ${src::qhsmtst.cpp}
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
//$endhead${src::qhsmtst.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "qhsmtst.hpp"

namespace QHSMTST {

//$declare${HSMs::QHsmTst} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${HSMs::QHsmTst} ...........................................................
class QHsmTst : public QP::QHsm {
private:
    bool m_foo;

public:
    QHsmTst()
      : QHsm(&initial)
    {}

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(s);
    Q_STATE_DECL(s1);
    Q_STATE_DECL(s11);
    Q_STATE_DECL(s2);
    Q_STATE_DECL(s21);
    Q_STATE_DECL(s211);
}; // class QHsmTst
//$enddecl${HSMs::QHsmTst} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

static QHsmTst l_hsmtst; // the only instance of the QHsmTst class

// global-scope definitions -----------------------------------------
QP::QHsm * const the_hsm = &l_hsmtst; // the opaque pointer

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.0.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${HSMs::QHsmTst} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${HSMs::QHsmTst} ...........................................................

//${HSMs::QHsmTst::SM} .......................................................
Q_STATE_DEF(QHsmTst, initial) {
    //${HSMs::QHsmTst::SM::initial}
    (void)e; // avoid compiler warning
    m_foo = 0U;
    BSP_display("top-INIT;");

    QS_SIG_DICTIONARY(A_SIG, this);
    QS_SIG_DICTIONARY(B_SIG, this);
    QS_SIG_DICTIONARY(C_SIG, this);
    QS_SIG_DICTIONARY(D_SIG, this);
    QS_SIG_DICTIONARY(E_SIG, this);
    QS_SIG_DICTIONARY(F_SIG, this);
    QS_SIG_DICTIONARY(G_SIG, this);
    QS_SIG_DICTIONARY(H_SIG, this);
    QS_SIG_DICTIONARY(I_SIG, this);
    QS_SIG_DICTIONARY(TERMINATE_SIG, this);
    QS_SIG_DICTIONARY(IGNORE_SIG, this);

    QS_FUN_DICTIONARY(&QHsmTst::s);
    QS_FUN_DICTIONARY(&QHsmTst::s1);
    QS_FUN_DICTIONARY(&QHsmTst::s11);
    QS_FUN_DICTIONARY(&QHsmTst::s2);
    QS_FUN_DICTIONARY(&QHsmTst::s21);
    QS_FUN_DICTIONARY(&QHsmTst::s211);

    return tran(&s2);
}

//${HSMs::QHsmTst::SM::s} ....................................................
Q_STATE_DEF(QHsmTst, s) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s}
        case Q_ENTRY_SIG: {
            BSP_display("s-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s}
        case Q_EXIT_SIG: {
            BSP_display("s-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::initial}
        case Q_INIT_SIG: {
            BSP_display("s-INIT;");
            status_ = tran(&s11);
            break;
        }
        //${HSMs::QHsmTst::SM::s::I}
        case I_SIG: {
            //${HSMs::QHsmTst::SM::s::I::[m_foo]}
            if (m_foo) {
                m_foo = 0U;
                BSP_display("s-I;");
                status_ = Q_RET_HANDLED;
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${HSMs::QHsmTst::SM::s::E}
        case E_SIG: {
            BSP_display("s-E;");
            status_ = tran(&s11);
            break;
        }
        //${HSMs::QHsmTst::SM::s::TERMINATE}
        case TERMINATE_SIG: {
            BSP_terminate(0);
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

//${HSMs::QHsmTst::SM::s::s1} ................................................
Q_STATE_DEF(QHsmTst, s1) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s::s1}
        case Q_ENTRY_SIG: {
            BSP_display("s1-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1}
        case Q_EXIT_SIG: {
            BSP_display("s1-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::initial}
        case Q_INIT_SIG: {
            BSP_display("s1-INIT;");
            status_ = tran(&s11);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::I}
        case I_SIG: {
            BSP_display("s1-I;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::D}
        case D_SIG: {
            //${HSMs::QHsmTst::SM::s::s1::D::[!m_foo]}
            if (!m_foo) {
                m_foo = true;
                BSP_display("s1-D;");
                status_ = tran(&s);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::A}
        case A_SIG: {
            BSP_display("s1-A;");
            status_ = tran(&s1);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::B}
        case B_SIG: {
            BSP_display("s1-B;");
            status_ = tran(&s11);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::F}
        case F_SIG: {
            BSP_display("s1-F;");
            status_ = tran(&s211);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::C}
        case C_SIG: {
            BSP_display("s1-C;");
            status_ = tran(&s2);
            break;
        }
        default: {
            status_ = super(&s);
            break;
        }
    }
    return status_;
}

//${HSMs::QHsmTst::SM::s::s1::s11} ...........................................
Q_STATE_DEF(QHsmTst, s11) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s::s1::s11}
        case Q_ENTRY_SIG: {
            BSP_display("s11-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::s11}
        case Q_EXIT_SIG: {
            BSP_display("s11-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::s11::H}
        case H_SIG: {
            BSP_display("s11-H;");
            status_ = tran(&s);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::s11::D}
        case D_SIG: {
            //${HSMs::QHsmTst::SM::s::s1::s11::D::[m_foo]}
            if (m_foo) {
                m_foo = false;
                BSP_display("s11-D;");
                status_ = tran(&s1);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${HSMs::QHsmTst::SM::s::s1::s11::G}
        case G_SIG: {
            BSP_display("s11-G;");
            status_ = tran(&s211);
            break;
        }
        default: {
            status_ = super(&s1);
            break;
        }
    }
    return status_;
}

//${HSMs::QHsmTst::SM::s::s2} ................................................
Q_STATE_DEF(QHsmTst, s2) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s::s2}
        case Q_ENTRY_SIG: {
            BSP_display("s2-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2}
        case Q_EXIT_SIG: {
            BSP_display("s2-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::initial}
        case Q_INIT_SIG: {
            BSP_display("s2-INIT;");
            status_ = tran(&s211);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::I}
        case I_SIG: {
            //${HSMs::QHsmTst::SM::s::s2::I::[!m_foo]}
            if (!m_foo) {
                m_foo = true;
                BSP_display("s2-I;");
                status_ = Q_RET_HANDLED;
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::F}
        case F_SIG: {
            BSP_display("s2-F;");
            status_ = tran(&s11);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::C}
        case C_SIG: {
            BSP_display("s2-C;");
            status_ = tran(&s1);
            break;
        }
        default: {
            status_ = super(&s);
            break;
        }
    }
    return status_;
}

//${HSMs::QHsmTst::SM::s::s2::s21} ...........................................
Q_STATE_DEF(QHsmTst, s21) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s::s2::s21}
        case Q_ENTRY_SIG: {
            BSP_display("s21-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21}
        case Q_EXIT_SIG: {
            BSP_display("s21-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::initial}
        case Q_INIT_SIG: {
            BSP_display("s21-INIT;");
            status_ = tran(&s211);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::G}
        case G_SIG: {
            BSP_display("s21-G;");
            status_ = tran(&s1);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::A}
        case A_SIG: {
            BSP_display("s21-A;");
            status_ = tran(&s21);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::B}
        case B_SIG: {
            BSP_display("s21-B;");
            status_ = tran(&s211);
            break;
        }
        default: {
            status_ = super(&s2);
            break;
        }
    }
    return status_;
}

//${HSMs::QHsmTst::SM::s::s2::s21::s211} .....................................
Q_STATE_DEF(QHsmTst, s211) {
    QP::QState status_;
    switch (e->sig) {
        //${HSMs::QHsmTst::SM::s::s2::s21::s211}
        case Q_ENTRY_SIG: {
            BSP_display("s211-ENTRY;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::s211}
        case Q_EXIT_SIG: {
            BSP_display("s211-EXIT;");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::s211::H}
        case H_SIG: {
            BSP_display("s211-H;");
            status_ = tran(&s);
            break;
        }
        //${HSMs::QHsmTst::SM::s::s2::s21::s211::D}
        case D_SIG: {
            BSP_display("s211-D;");
            status_ = tran(&s21);
            break;
        }
        default: {
            status_ = super(&s21);
            break;
        }
    }
    return status_;
}
//$enddef${HSMs::QHsmTst} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

} // namespace QHSMTST
