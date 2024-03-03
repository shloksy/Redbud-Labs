/*$file${.::game.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: game.qm
* File:  ${.::game.h}
*
* This code has been generated by QM 5.2.4 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${.::game.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef GAME_H
#define GAME_H

enum GameSignals { /* signals used in the game */
    TIME_TICK_SIG = Q_USER_SIG, /* published from tick ISR */
    PLAYER_TRIGGER_SIG, /* published by Player (ISR) to trigger the Missile */
    PLAYER_QUIT_SIG,    /* published by Player (ISR) to quit the game */
    GAME_OVER_SIG,      /* published by Ship when it finishes exploding */

    /* insert other published signals here ... */
    MAX_PUB_SIG,        /* the last published signal */

    PLAYER_SHIP_UP_SIG,   /* posted by Player (ISR) to Ship to move it up  */
    PLAYER_SHIP_DOWN_SIG, /* posted by Player (ISR) to Ship to let if fall */

    BLINK_TIMEOUT_SIG,    /* signal for Tunnel's blink timeout event */
    SCREEN_TIMEOUT_SIG,   /* signal for Tunnel's screen timeout event */

    TAKE_OFF_SIG,  /* from Tunnel to Ship to grant permission to take off */
    HIT_WALL_SIG,  /* from Tunnel to Ship when Ship hits the wall */
    HIT_MINE_SIG,  /* from Mine to Ship or Missile when it hits the mine */
    SHIP_IMG_SIG,  /* from Ship to the Tunnel to draw and check for hits */
    MISSILE_IMG_SIG, /* from Missile the Tunnel to draw and check for hits */
    MINE_IMG_SIG,  /* sent by Mine to the Tunnel to draw the mine */
    MISSILE_FIRE_SIG,   /* sent by Ship to the Missile to fire */
    DESTROYED_MINE_SIG, /* from Missile to Ship when Missile destroyed Mine */
    EXPLOSION_SIG, /* from any exploding object to render the explosion */
    MINE_PLANT_SIG,     /* from Tunnel to the Mine to plant it */
    MINE_DISABLED_SIG,  /* from Mine to Tunnel when it becomes disabled */
    MINE_RECYCLE_SIG,   /* sent by Tunnel to Mine to recycle the mine */
    SCORE_SIG, /* from Ship to Tunnel to adjust game level based on score */

    MAX_SIG /* the last signal (keep always last) */
};

#define GAME_TUNNEL_WIDTH          BSP_SCREEN_WIDTH
#define GAME_TUNNEL_HEIGHT         (BSP_SCREEN_HEIGHT - 10U)
#define GAME_MINES_MAX             5U
#define GAME_MINES_DIST_MIN        10U
#define GAME_SPEED_X               1U
#define GAME_MISSILE_SPEED_X       2U
#define GAME_SHIP_X                10U
#define GAME_SHIP_Y                (GAME_TUNNEL_HEIGHT / 2U)
#define GAME_WALLS_GAP_Y           50U
#define GAME_WALLS_MIN_GAP_Y       20U

enum GameBitmapIds {
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

/* declare shared facilities */
/*$declare${Shared} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Shared::ObjectPosEvt} ..................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t x;
    uint8_t y;
} ObjectPosEvt;

/*${Shared::ObjectImageEvt} ................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t x;
    int8_t y;
    uint8_t bmp;
} ObjectImageEvt;

/*${Shared::MineEvt} .......................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t id;
} MineEvt;

/*${Shared::ScoreEvt} ......................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint16_t score;
} ScoreEvt;

/*${Shared::AO_Tunnel} .....................................................*/
/* opaque AO pointer */
extern QActive * const AO_Tunnel;

/*${Shared::AO_Ship} .......................................................*/
/* opaque AO pointer */
extern QActive * const AO_Ship;

/*${Shared::AO_Missile} ....................................................*/
/* opaque AO pointer */
extern QActive * const AO_Missile;

/*${Shared::Tunnel_ctor_call} ..............................................*/
void Tunnel_ctor_call(void);

/*${Shared::Ship_ctor_call} ................................................*/
void Ship_ctor_call(void);

/*${Shared::Missile_ctor_call} .............................................*/
void Missile_ctor_call(void);

/*${Shared::Mine1_ctor_call} ...............................................*/
QHsm * Mine1_ctor_call(uint8_t id);

/*${Shared::Mine2_ctor_call} ...............................................*/
QHsm * Mine2_ctor_call(uint8_t id);
/*$enddecl${Shared} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#endif  /* GAME_H */

