//$file${.::mine1.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: game.qm
// File:  ${.::mine1.cpp}
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
//$endhead${.::mine1.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"

Q_DEFINE_THIS_FILE

// encapsulated delcaration of the Mine1 HSM ---------------------------------
//$declare${AOs::Mine1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Mine1} ..............................................................
class Mine1 : public QP::QHsm {
private:
    uint8_t m_x;
    uint8_t m_y;

public:
    uint8_t m_exp_ctr;

public:
    Mine1();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(unused);
    Q_STATE_DECL(used);
    Q_STATE_DECL(exploding);
    Q_STATE_DECL(planted);
}; // class Mine1

} // namespace GAME
//$enddecl${AOs::Mine1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

namespace GAME {

// local objects -------------------------------------------------------------
static Mine1 l_mine1[GAME_MINES_MAX]; // a pool of type-1 mines

//............................................................................
QP::QHsm *Mine1_getInst(uint8_t id) {
    Q_REQUIRE(id < GAME_MINES_MAX);
    return &l_mine1[id];
}

// helper function to provide the ID of this mine ............................
static inline uint8_t MINE_ID(Mine1 const * const me) {
    return static_cast<uint8_t>(me - l_mine1);
}

} // namespace GAME

// Mine1 class definition ----------------------------------------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.0.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${AOs::Mine1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Mine1} ..............................................................

//${AOs::Mine1::Mine1} .......................................................
Mine1::Mine1()
 : QHsm(Q_STATE_CAST(&Mine1::initial))
{}

//${AOs::Mine1::SM} ..........................................................
Q_STATE_DEF(Mine1, initial) {
    //${AOs::Mine1::SM::initial}
    static bool dict_sent = false;
    if (!dict_sent) {
        dict_sent = true;
        QS_OBJ_DICTIONARY(&l_mine1[0]); // obj. dictionaries for Mine1 pool
        QS_OBJ_DICTIONARY(&l_mine1[1]);
        QS_OBJ_DICTIONARY(&l_mine1[2]);
        QS_OBJ_DICTIONARY(&l_mine1[3]);
        QS_OBJ_DICTIONARY(&l_mine1[4]);

        QS_FUN_DICTIONARY(&Mine1::initial);// fun. dictionaries for Mine1 HSM
        QS_FUN_DICTIONARY(&Mine1::unused);
        QS_FUN_DICTIONARY(&Mine1::used);
        QS_FUN_DICTIONARY(&Mine1::planted);
        QS_FUN_DICTIONARY(&Mine1::exploding);
    }
    // local signals
    QS_SIG_DICTIONARY(MINE_PLANT_SIG,    this);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG, this);
    QS_SIG_DICTIONARY(MINE_RECYCLE_SIG,  this);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,      this);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,   this);

    QS_FUN_DICTIONARY(&Mine1::unused);
    QS_FUN_DICTIONARY(&Mine1::used);
    QS_FUN_DICTIONARY(&Mine1::exploding);
    QS_FUN_DICTIONARY(&Mine1::planted);

    return tran(&unused);
}

//${AOs::Mine1::SM::unused} ..................................................
Q_STATE_DEF(Mine1, unused) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine1::SM::unused::MINE_PLANT}
        case MINE_PLANT_SIG: {
            m_x = Q_EVT_CAST(ObjectPosEvt)->x;
            m_y = Q_EVT_CAST(ObjectPosEvt)->y;
            status_ = tran(&planted);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${AOs::Mine1::SM::used} ....................................................
Q_STATE_DEF(Mine1, used) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine1::SM::used}
        case Q_EXIT_SIG: {
            // tell the Tunnel that this mine is becoming disabled
            AO_Tunnel->POST(Q_NEW(MineEvt, MINE_DISABLED_SIG, MINE_ID(this)), this);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Mine1::SM::used::MINE_RECYCLE}
        case MINE_RECYCLE_SIG: {
            status_ = tran(&unused);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${AOs::Mine1::SM::used::exploding} .........................................
Q_STATE_DEF(Mine1, exploding) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine1::SM::used::exploding}
        case Q_ENTRY_SIG: {
            m_exp_ctr = 0U;
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Mine1::SM::used::exploding::TIME_TICK}
        case TIME_TICK_SIG: {
            //${AOs::Mine1::SM::used::exploding::TIME_TICK::[stillonscreen?]}
            if ((m_x >= GAME_SPEED_X) && (m_exp_ctr < 15)) {
                ++m_exp_ctr;  // advance the explosion counter
                m_x -= GAME_SPEED_X; // move explosion by 1 step

                // tell the Game to render the current stage of Explosion
                AO_Tunnel->POST(Q_NEW(ObjectImageEvt, EXPLOSION_SIG,
                                      m_x + 1U, (int8_t)((int)m_y - 4 + 2),
                                      EXPLOSION0_BMP + (m_exp_ctr >> 2)),
                                this);
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Mine1::SM::used::exploding::TIME_TICK::[else]}
            else {
                status_ = tran(&unused);
            }
            break;
        }
        default: {
            status_ = super(&used);
            break;
        }
    }
    return status_;
}

//${AOs::Mine1::SM::used::planted} ...........................................
Q_STATE_DEF(Mine1, planted) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine1::SM::used::planted::TIME_TICK}
        case TIME_TICK_SIG: {
            //${AOs::Mine1::SM::used::planted::TIME_TICK::[m_x>=GAME_SPEED_X]}
            if (m_x >= GAME_SPEED_X) {
                m_x -= GAME_SPEED_X; // move the mine 1 step
                // tell the Tunnel to draw the Mine
                AO_Tunnel->POST(Q_NEW(ObjectImageEvt, MINE_IMG_SIG,
                                      m_x, m_y, MINE1_BMP),
                                this);
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Mine1::SM::used::planted::TIME_TICK::[else]}
            else {
                status_ = tran(&unused);
            }
            break;
        }
        //${AOs::Mine1::SM::used::planted::SHIP_IMG}
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            //${AOs::Mine1::SM::used::planted::SHIP_IMG::[collisionwithMINE1_BMP?]}
            if (do_bitmaps_overlap(MINE1_BMP, m_x, m_y, bmp, x, y)) {
                static MineEvt const mine1_hit(HIT_MINE_SIG, 1U);
                AO_Ship->POST(&mine1_hit, this);
                // go straight to 'disabled' and let the Ship do
                // the exploding
                status_ = tran(&unused);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${AOs::Mine1::SM::used::planted::MISSILE_IMG}
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            //${AOs::Mine1::SM::used::planted::MISSILE_IMG::[collisionwithMINE1_BMP?]}
            if (do_bitmaps_overlap(MINE1_BMP, m_x, m_y, bmp, x, y)) {
                static ScoreEvt const mine1_destroyed(DESTROYED_MINE_SIG, 25U);
                AO_Missile->POST(&mine1_destroyed, this);
                status_ = tran(&exploding);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        default: {
            status_ = super(&used);
            break;
        }
    }
    return status_;
}

} // namespace GAME
//$enddef${AOs::Mine1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^