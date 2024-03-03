//$file${.::game.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: game.qm
// File:  ${.::game.hpp}
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
//$endhead${.::game.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef GAME_HPP
#define GAME_HPP

namespace GAME {

enum GameSignals { // signals used in the game
    TIME_TICK_SIG = QP::Q_USER_SIG, // published from tick ISR
    PLAYER_TRIGGER_SIG, // published by Player (ISR) to trigger the Missile
    PLAYER_QUIT_SIG,    // published by Player (ISR) to quit the game
    GAME_OVER_SIG,      // published by Ship when it finishes exploding

    // insert other published signals here ...
    MAX_PUB_SIG,        // the last published signal

    PLAYER_SHIP_MOVE_SIG, // posted by Player (ISR) to the Ship to move it


    BLINK_TIMEOUT_SIG,  // signal for Tunnel's blink timeout event
    SCREEN_TIMEOUT_SIG, // signal for Tunnel's screen timeout event

    TAKE_OFF_SIG,       // from Tunnel to Ship to grant permission to take off
    HIT_WALL_SIG,       // from Tunnel to Ship when Ship hits the wall
    HIT_MINE_SIG,       // from Mine to Ship or Missile when it hits the mine
    SHIP_IMG_SIG,       // from Ship to the Tunnel to draw and check for hits
    MISSILE_IMG_SIG,    // from Missile the Tunnel to draw and check for hits
    MINE_IMG_SIG,       // sent by Mine to the Tunnel to draw the mine
    MISSILE_FIRE_SIG,   // sent by Ship to the Missile to fire
    DESTROYED_MINE_SIG, // from Missile to Ship when Missile destroyed Mine
    EXPLOSION_SIG,      // from any exploding object to render the explosion
    MINE_PLANT_SIG,     // from Tunnel to the Mine to plant it
    MINE_DISABLED_SIG,  // from Mine to Tunnel when it becomes disabled
    MINE_RECYCLE_SIG,   // sent by Tunnel to Mine to recycle the mine
    SCORE_SIG, // from Ship to Tunnel to adjust game level based on score

    MAX_SIG             // the last signal (keep always last)
};

enum GameBitmapIds {
    PRESS_BUTTON_BMP,
    SHIP_BMP,
    MISSILE_BMP,
    MINE1_BMP,
    MINE2_BMP,
    MINE2_MISSILE_BMP,
    EXPLOSION0_BMP,
    EXPLOSION1_BMP,
    EXPLOSION2_BMP,
    EXPLOSION3_BMP,
    MAX_BMP
};

// obtain instances of the Mines orthogonal components
QP::QHsm *Mine1_getInst(uint8_t id);
QP::QHsm *Mine2_getInst(uint8_t id);

} // namespace GAME

//$declare${Events::ObjectPosEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Events::ObjectPosEvt} ....................................................
class ObjectPosEvt : public QP::QEvt {
public:
    uint8_t x;
    uint8_t y;

public:
    ObjectPosEvt(
        QP::QSignal sig,
        uint8_t x_p,
        uint8_t y_p)
    : QEvt(sig),
      x(x_p),
      y(y_p)
    {}
}; // class ObjectPosEvt

} // namespace GAME
//$enddecl${Events::ObjectPosEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${Events::ObjectImageEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Events::ObjectImageEvt} ..................................................
class ObjectImageEvt : public QP::QEvt {
public:
    uint8_t x;
    int8_t y;
    uint8_t bmp;

#if defined __LP64__
    uint32_t pad;
#endif //  defined __LP64__

public:
    ObjectImageEvt(
        QP::QSignal sig,
        uint8_t x_p,
        uint8_t y_p,
        uint8_t bmp_p)
    : QEvt(sig),
      x(x_p),
      y(y_p),
      bmp(bmp_p)
    {}
}; // class ObjectImageEvt

} // namespace GAME
//$enddecl${Events::ObjectImageEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${Events::MineEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Events::MineEvt} .........................................................
class MineEvt : public QP::QEvt {
public:
    uint8_t id;

public:
    MineEvt(
        QP::QSignal sig,
        uint8_t id_p)
    : QEvt(sig),
      id(id_p)
    {}
}; // class MineEvt

} // namespace GAME
//$enddecl${Events::MineEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${Events::ScoreEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Events::ScoreEvt} ........................................................
class ScoreEvt : public QP::QEvt {
public:
    uint16_t score;

public:
    ScoreEvt(
        QP::QSignal sig,
        uint16_t score_p)
    : QEvt(sig),
      score(score_p)
    {}
}; // class ScoreEvt

} // namespace GAME
//$enddecl${Events::ScoreEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#define GAME_SCREEN_WIDTH          BSP_SCREEN_WIDTH
#define GAME_SCREEN_HEIGHT         BSP_SCREEN_HEIGHT
#define GAME_MINES_MAX             5U
#define GAME_MINES_DIST_MIN        10U
#define GAME_SPEED_X               1U
#define GAME_MISSILE_SPEED_X       2U
#define GAME_SHIP_X                10U
#define GAME_SHIP_Y                (GAME_SCREEN_HEIGHT / 2U)

// opaque pointers to active objects in the application
//$declare${AOs::AO_Tunnel} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::AO_Tunnel} ..........................................................
extern QP::QActive * const AO_Tunnel;

} // namespace GAME
//$enddecl${AOs::AO_Tunnel} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${AOs::AO_Ship} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::AO_Ship} ............................................................
extern QP::QActive * const AO_Ship;

} // namespace GAME
//$enddecl${AOs::AO_Ship} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${AOs::AO_Missile} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::AO_Missile} .........................................................
extern QP::QActive * const AO_Missile;

} // namespace GAME
//$enddecl${AOs::AO_Missile} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// helper function for all AOs
//$declare${AOs::do_bitmaps_overlap} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::do_bitmaps_overlap} .................................................
bool do_bitmaps_overlap(
    uint8_t bmp_id1,
    uint8_t x1,
    uint8_t y1,
    uint8_t bmp_id2,
    uint8_t x2,
    uint8_t y2);

} // namespace GAME
//$enddecl${AOs::do_bitmaps_overlap} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif  // GAME_HPP