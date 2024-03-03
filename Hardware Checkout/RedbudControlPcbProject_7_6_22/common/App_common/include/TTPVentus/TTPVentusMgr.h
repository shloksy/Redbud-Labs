/**
 * @file TTPVentusMgr.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSMGR_H_
#define APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSMGR_H_

#include "qpcpp.hpp"
#include "qphelper.h"
#include "bsp.h"

#include "InterruptEntry.h"

#include "TTPVentus/TTPVentusUartProtocol.h"

#include "usart.h"

#include <stdint.h>
#include <string.h>

namespace COMMON {
namespace TTPV {

// Sample traits
//struct TTPVTraits {
//    static constexpr UART_HandleTypeDef *uart_handle = nullptr;
//};

template<typename TTPVTraits>
class TTPVentusMgr : public QP::QActive, TTPVTraits {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        UART_RX_COMPLETE_SIG,
        UART_TX_COMPLETE_SIG,
        UART_ERROR_SIG,
        PERIODIC_TX_SIG
    };

    uint8_t rx_char;
    QP::QTimeEvt PeriodicTxTimer;

    static constexpr uint32_t PeriodicTxTicks = 500 / COMMON::BSP::MsPerBspTick;

public:
    TTPVentusMgr()
    : QActive(Q_STATE_CAST(&TTPVentusMgr::initial)),
      rx_char(0),
      PeriodicTxTimer(this, PERIODIC_TX_SIG, 0U)
    {
        ;
    }

    // NOTE: The following are called from an interrupt context!
    void UARTRxComplete(InterruptEntry = {}) {
        PostUARTRxComplete();
        UART_Start_Receive_IT(TTPVTraits::uart_handle, &rx_char, 1);
    }
    void UARTTxComplete(InterruptEntry = {}) {
        PostUARTTxComplete();
    }
    void UARTError(InterruptEntry = {}) {
        PostUARTError();
    }

private:
    void PostUARTRxComplete() {
        auto e = Q_NEW(Uint8Evt, UART_RX_COMPLETE_SIG);
        e->data = rx_char;
        POST(e, this);
    }
    void PostUARTTxComplete() {
        POST(Q_NEW(Uint8Evt, UART_TX_COMPLETE_SIG), this);
    }
    void PostUARTError() {
        POST(Q_NEW(Uint8Evt, UART_ERROR_SIG), this);
    }
    void PostStartOperation() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }

protected:
    //! initial
    Q_STATE_DEF_INTERNAL(TTPVentusMgr, initial) {
        (void)e;

        return tran(&startup);
    }

    //! running
    Q_STATE_DEF_INTERNAL(TTPVentusMgr, running) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                UART_Start_Receive_IT(TTPVTraits::uart_handle, &rx_char, 1);

                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case UART_RX_COMPLETE_SIG: {
                QS_BEGIN_ID(TTPVentusMgrMsg, getPrio())
#ifdef Q_SPY
                    char print_str[2] = {static_cast<const Uint8Evt *>(e)->data, 0};
#endif
                    QS_STR(print_str);
                QS_END()
                status_ = Q_RET_HANDLED;
                break;
            }

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

    //! running::startup
    Q_STATE_DEF_INTERNAL(TTPVentusMgr, startup) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                PostStartOperation();
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case START_OPERATION_SIG:
                status_ = tran(&read_major_version);
                break;

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

    //! running::read_major_version
    Q_STATE_DEF_INTERNAL(TTPVentusMgr, read_major_version) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                PeriodicTxTimer.armX(PeriodicTxTicks, PeriodicTxTicks);
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                PeriodicTxTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            case PERIODIC_TX_SIG:
                HAL_UART_Transmit_IT(TTPVTraits::uart_handle, reinterpret_cast<const uint8_t *>(read_major_version_string), strlen(read_major_version_string));
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&running);
                break;
        }

        return status_;
    }

};

} // namespace TTPV
} // namespace COMMON



#endif /* APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSMGR_H_ */
