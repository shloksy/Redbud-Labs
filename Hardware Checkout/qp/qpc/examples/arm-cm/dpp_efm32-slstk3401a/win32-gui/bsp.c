/*****************************************************************************
* Product: DPP example, Win32-GUI
* Last updated for version: 6.9.3
* Date of the Last Update:  2021-03-03
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

#include "qwin_gui.h" /* QWIN GUI */
#include "resource.h" /* GUI resource IDs generated by the resource editior */

#include "safe_std.h" /* portable "safe" <stdio.h>/<string.h> facilities */

Q_DEFINE_THIS_FILE

/* local variables ---------------------------------------------------------*/
static HINSTANCE l_hInst;   /* this application instance */
static HWND      l_hWnd;    /* main window handle */
static LPSTR     l_cmdLine; /* the command line string */

static SegmentDisplay   l_philos;   /* SegmentDisplay to show Philo status */
static OwnerDrawnButton l_pauseBtn; /* owner-drawn button */

static unsigned  l_rnd;  /* random seed */

#ifdef Q_SPY
    enum {
        PHILO_STAT = QS_USER
    };
    static uint8_t const l_clock_tick = 0U;
#endif

/* Local functions ---------------------------------------------------------*/
static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg,
                                WPARAM wParam, LPARAM lParam);

/*..........................................................................*/
/* thread function for running the application main_gui() */
static DWORD WINAPI appThread(LPVOID par) {
    (void)par; /* unused parameter */
    return (DWORD)main_gui(); /* run the QF application */
}

/*--------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
                   LPSTR cmdLine, int iCmdShow)
{
    HWND hWnd;
    MSG  msg;

    (void)hPrevInst; /* unused parameter */

    l_hInst   = hInst;   /* save the application instance */
    l_cmdLine = cmdLine; /* save the command line string */

    //AllocConsole();

    /* create the main custom dialog window */
    hWnd = CreateCustDialog(hInst, IDD_APPLICATION, NULL,
                            &WndProc, "MY_CLASS");
    ShowWindow(hWnd, iCmdShow);  /* show the main window */

    /* enter the message loop... */
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //FreeConsole();
    BSP_terminate(0);

    return msg.wParam;
}

/*..........................................................................*/
static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg,
                                WPARAM wParam, LPARAM lParam)
{
    switch (iMsg) {

        /* Perform initialization upon cration of the main dialog window
        * NOTE: Any child-windows are NOT created yet at this time, so
        * the GetDlgItem() function can't be used (it will return NULL).
        */
        case WM_CREATE: {
            l_hWnd = hWnd; /* save the window handle */

            /* initialize the owner-drawn buttons...
            * NOTE: must be done *before* the first drawing of the buttons,
            * so WM_INITDIALOG is too late.
            */
            OwnerDrawnButton_init(&l_pauseBtn, IDC_PAUSE,
                LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_UP)),
                LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_DWN)),
                LoadCursor(NULL, IDC_HAND));
            return 0;
        }

        /* Perform initialization after all child windows have been created */
        case WM_INITDIALOG: {

            SegmentDisplay_init(&l_philos,
                     N_PHILO,          /* N_PHILO "segments" for the Philos */
                     3U);         /* 3 bitmaps (for thinking/hungry/eating) */
            SegmentDisplay_initSegment(&l_philos, 0U, IDC_PHILO_0);
            SegmentDisplay_initSegment(&l_philos, 1U, IDC_PHILO_1);
            SegmentDisplay_initSegment(&l_philos, 2U, IDC_PHILO_2);
            SegmentDisplay_initSegment(&l_philos, 3U, IDC_PHILO_3);
            SegmentDisplay_initSegment(&l_philos, 4U, IDC_PHILO_4);
            SegmentDisplay_initBitmap(&l_philos,
                 0U, LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_THINKING)));
            SegmentDisplay_initBitmap(&l_philos,
                 1U, LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_HUNGRY)));
            SegmentDisplay_initBitmap(&l_philos,
                 2U, LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_EATING)));

            /* --> QP: spawn the application thread to run main_gui() */
            Q_ALLEGE(CreateThread(NULL, 0, &appThread, NULL, 0, NULL)
                     != (HANDLE)0);

            SetDlgItemTextA(hWnd, IDC_EDIT1, "Edit1");
            SetDlgItemTextA(hWnd, IDC_EDIT2, "Edit2");
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        /* commands from child controls and menus... */
        case WM_COMMAND: {
            switch (wParam) {
                case IDOK:
                case IDCANCEL: {
                    PostQuitMessage(0);
                    break;
                }
                case IDC_PAUSE: { /* owner-drawn button(s) */
                    SetFocus(hWnd);
                    break;
                }
                case IDC_BUTTON1: { /* regular button */
                    char buf[32];
                    GetDlgItemTextA(hWnd, IDC_EDIT1, buf, sizeof(buf));
                    SetDlgItemTextA(hWnd, IDC_EDIT2, buf);
                    break;
                }
            }
            return 0;
        }

        /* drawing of owner-drawn buttons... */
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
            switch (pdis->CtlID) {
                case IDC_PAUSE: { /* PAUSE owner-drawn button */
                    switch (OwnerDrawnButton_draw(&l_pauseBtn,pdis)) {
                        case BTN_DEPRESSED: {
                            static QEvt const pe = { PAUSE_SIG, 0U, 0U };
                            QACTIVE_POST(AO_Table, &pe, (void *)0);
                            break;
                        }
                        case BTN_RELEASED: {
                            static QEvt const se = { SERVE_SIG, 0U, 0U };
                            QACTIVE_POST(AO_Table, &se, (void *)0);
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
            }
            return 0;
        }

        /* mouse input... */
        case WM_MOUSEWHEEL: {
            return 0;
        }

        /* keyboard input... */
        case WM_KEYDOWN: {
            return 0;
        }
    }
    return DefWindowProc(hWnd, iMsg, wParam, lParam) ;
}
/*..........................................................................*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC, 30); /* set the desired tick rate */
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QF_onClockTick(void) {
    QTIMEEVT_TICK_X(0U, &l_clock_tick); /* perform the QF clock tick processing */

    QS_RX_INPUT(); /* handle the QS-RX input */
    QS_OUTPUT();   /* handle the QS output */
}

/*..........................................................................*/
Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    char message[80];
    QF_stop(); /* stop ticking */

    QS_ASSERTION(module, loc, 10000U); /* report assertion to QS */
    SNPRINTF_S(message, Q_DIM(message) - 1U,
               "Assertion failed in module %s location %d", module, loc);
    MessageBox(l_hWnd, message, "!!! ASSERTION !!!",
               MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
    PostQuitMessage(-1);
}

/*..........................................................................*/
void BSP_init(void) {
    if (QS_INIT(l_cmdLine) == 0U) { /* QS initialization failed? */
        MessageBox(l_hWnd,
                   "Cannot connect to QSPY via TCP/IP\n"
                   "Please make sure that 'qspy -t' is running",
                   "QS_INIT() Error",
                   MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
    }

    /* send the QS dictionaries... */
    QS_OBJ_DICTIONARY(&l_clock_tick);
    QS_USR_DICTIONARY(PHILO_STAT);

    /* setup the QS filters... */
    QS_GLB_FILTER(QS_ALL_RECORDS);
    QS_GLB_FILTER(-QS_QF_TICK);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    QF_stop(); /* stop the main QF application */

    /* cleanup all QWIN resources... */
    OwnerDrawnButton_xtor(&l_pauseBtn); /* cleanup the l_pauseBtn resources */
    SegmentDisplay_xtor(&l_philos);     /* cleanup the l_philos resources */

    /* end the main dialog */
    EndDialog(l_hWnd, result);
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    UINT bitmapNum = 0;

    Q_REQUIRE(n < N_PHILO);

    switch (stat[0]) {
        case 't': bitmapNum = 0U; break;
        case 'h': bitmapNum = 1U; break;
        case 'e': bitmapNum = 2U; break;
        default: Q_ERROR();  break;
    }
    /* set the "segment" # n to the bitmap # 'bitmapNum' */
    SegmentDisplay_setSegment(&l_philos, (UINT)n, bitmapNum);

    QS_BEGIN_ID(PHILO_STAT, AO_Philo[n]->prio) /* app-specific record */
        QS_U8(1, n);  /* Philosopher number */
        QS_STR(stat); /* Philosopher status */
    QS_END()
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    char buf[16];
    LoadString(l_hInst,
        (paused != 0U) ? IDS_PAUSED : IDS_RUNNING, buf, Q_DIM(buf));
    SetDlgItemText(l_hWnd, IDC_PAUSED, buf);
}
/*..........................................................................*/
uint32_t BSP_random(void) {  /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}


/*--------------------------------------------------------------------------*/
#ifdef Q_SPY /* define QS callbacks */

/*! callback function to execute user commands */
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    switch (cmdId) {
       case 0U: {
           break;
       }
       default:
           break;
    }

    /* unused parameters */
    (void)param1;
    (void)param2;
    (void)param3;
}

#endif /* Q_SPY */
/*--------------------------------------------------------------------------*/
