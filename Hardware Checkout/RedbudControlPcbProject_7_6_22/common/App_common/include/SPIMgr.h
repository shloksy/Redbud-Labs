/**
 * @file SPIMgr.h
 *
 * @author awong
 *
 * QP Interface:
 * Subscribe:
 * - SPI_TRANSACTION_SIG:
 *   Event containing information about the SPI transaction (such as setup and hold times,
 *   write buffer, transaction length, QP signal to publish upon completion of transaction).
 * Publish:
 * - {qp_complete_signal}
 *   This published signal comes from the SPI_TRANSACTION_SIG event.  This is application
 *   specific and allows some form of unicasting to a particular active object.
 *
 * State Machine:
 * - idle
 *   No transaction is currently being handled.  Immediately acts on a received transaction.
 * - busy
 *   SPI transaction in progress.  Defers any incoming transactions until next time in idle state.
 *   - chip_select_setup_time
 *     Sets chip select low and then delays based on transaction configured setup time.
 *   - transaction_in_progress
 *     Sends all SPI data and waits for SPI complete event to be posted by ISR.
 *   - chip_select_hold_time
 *     Delays based on transaction configured setup time and then sets chip select high.
 */

#ifndef INCLUDE_SPIMGR_H_
#define INCLUDE_SPIMGR_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"

#include "InterruptEntry.h"

#include "SPIPeriph.h"
#include "QPEvt/SPIEvt.h"

// Note: Q_DEFINE_THIS_MODULE("") must be defined before SPIMgr.h is included in a source file.

namespace COMMON {
namespace SPI {

// SPITraits template example with nothing filled in.  The application must
// fill in all of these parameters.
//struct SPITraits {
//    static constexpr enum_t SPI_TRANSACTION_SIG = 0;
//
//    static constexpr SPI_HandleTypeDef *spi_handle = nullptr;
//    static constexpr TIM_HandleTypeDef *tim_handle = nullptr;
//
//    // Depth of the internal defer/recall QP queue within SPIMgr.
//    static constexpr size_t TransactionQueueDepth = 0;
//    // Depth of the SPI data buffers used within for SPIMgr.
//    static constexpr size_t MaxSPITransactionLength = 0;
//};

template<typename SPI_traits>
class SPIMgr : public QP::QActive {
private:
    enum InternalSignals {
        TRANSACTION_DONE_SIG = MAX_PUB_SIG,
        TIMER_PERIOD_ELAPSED_SIG
    };

    // Configuration:
    static constexpr size_t TransactionQueueDepth = SPI_traits::TransactionQueueDepth;
    static constexpr size_t MaxSPITransactionLength = SPI_traits::MaxSPITransactionLength;
    using spi_buffer = std::array<uint8_t, MaxSPITransactionLength>;

    // Note: These are arbitrarily set to catch errors with the configured setup and hold times.
    // 5 ms seems like a really long time to set a setup/hold time.
    static constexpr uint32_t MaximumAcceptableSetupTime_us = 5000;
    static constexpr uint32_t MaximumAcceptableHoldTime_us = 5000;

private:
    QP::QEQueue SpiTransactionQueue;
    QP::QEvt const * SpiTransactionQSto[TransactionQueueDepth];

    struct SPITransaction {
        enum_t qp_complete_signal;
        GpioPin chip_select_pin;
        SpiConfig config;
        spi_buffer write_buf;
        spi_buffer read_buf;
        size_t transaction_len;
    };

    SPITransaction CurrentTransaction;

    spi_buffer empty_buf;

public:
    SPIMgr() :
        QActive(Q_STATE_CAST(&SPIMgr::initial)),
        CurrentTransaction{},
        empty_buf{0}
        {
            SpiTransactionQueue.init(SpiTransactionQSto, Q_DIM(SpiTransactionQSto));

            // Note: Required that the spi_handle won't be used before being initialized in main().
        }

        // NOTE: Called from in an interrupt context!
        void SpiTransactionComplete(InterruptEntry = {}) {
            postTransactionDone();
        }

        // NOTE: Called from in an interrupt context!
        void TimerPeriodElapsed(InterruptEntry = {}) {
            postTimerPeriodElapsed();
        }

private:
        void setCurrentTransaction(const TransactionEvent * e);

        void TransactionDoneEvent() {
            QS_BEGIN_ID(SPIMgrMsg, getPrio())
                QS_STR("Complete.");
            QS_END()
            QS_BEGIN_ID(SPIMgrMsg, getPrio())
                QS_STR("W");
                for (size_t i = 0; i < CurrentTransaction.transaction_len; i++) {
                    QS_U32_HEX(2, CurrentTransaction.write_buf[i]);
                }
            QS_END()
            QS_BEGIN_ID(SPIMgrMsg, getPrio())
                QS_STR("R");
                for (size_t i = 0; i < CurrentTransaction.transaction_len; i++) {
                    QS_U32_HEX(2, CurrentTransaction.read_buf[i]);
                }
            QS_END()
            //! @todo For now, no handling of errors on the SPI transaction.
            publishTransactionCompleteEvent(CurrentTransaction.qp_complete_signal,
                    ErrorCode::NoError,
                    CurrentTransaction.read_buf,
                    CurrentTransaction.transaction_len);
        }
        void startTransaction() {
            //! @todo More gracefully handle this error?
            Q_ASSERT(HAL_OK == HAL_SPI_TransmitReceive_IT(SPI_traits::spi_handle,
                    CurrentTransaction.write_buf.data(),
                    CurrentTransaction.read_buf.data(),
                    CurrentTransaction.transaction_len));
        }

        void publishErrorEvent(const enum_t qp_complete_signal,
                               const ErrorCode error_code) {
            publishTransactionCompleteEvent(qp_complete_signal,
                    error_code,
                    empty_buf,
                    0);
        }
        void publishTransactionCompleteEvent(const enum_t qp_complete_signal,
                                             const ErrorCode error_code,
                                             const spi_buffer &read_buf,
                                             const size_t transaction_len);
        void postTransactionDone() {
            POST(Q_NEW(QP::QEvt, TRANSACTION_DONE_SIG), this);
        }

        void postTimerPeriodElapsed() {
            POST(Q_NEW(QP::QEvt, TIMER_PERIOD_ELAPSED_SIG), this);
        }

        void EnableCurrentTransactionChipSelect(bool enable);

        //! @todo Move to a more generic driver module.
        void StartChipSelectTimer(uint32_t timeout_us) {
            // Set in one-pulse mode.
            SPI_traits::tim_handle->Instance->CR1 |= TIM_CR1_OPM;

            SPI_traits::tim_handle->Instance->CNT = 0;

            // Set auto reload to timeout.
            SPI_traits::tim_handle->Instance->ARR = timeout_us * 5;

            // Disable update event interrupt.
            SPI_traits::tim_handle->Instance->DIER = 0;

            // Generate an update event.
            SPI_traits::tim_handle->Instance->EGR |= TIM_EGR_UG;

            // Clear status register.
            SPI_traits::tim_handle->Instance->SR = 0;

            //HAL_TIM_OnePulse_Start_IT(SPI_traits::tim_handle, TIM_CHANNEL_1);
            HAL_TIM_Base_Start_IT(SPI_traits::tim_handle);
        }

        //! @todo Move to a more generic driver module.
        void StopChipSelectTimer() {
            HAL_TIM_Base_Stop_IT(SPI_traits::tim_handle);
        }

        //! @todo Move to a more generic driver module.
        void ConfigureClockPolarity(const ClockPolarity clock_polarity);
        //! @todo Move to a more generic driver module.
        void ConfigureClockPhase(const ClockPhase clock_phase);

protected:
        Q_STATE_DEF_INTERNAL(SPIMgr, initial) {
            (void)e;

            subscribe(SPI_traits::SPI_TRANSACTION_SIG);

            return tran(&idle);
        }

        //! idle
        Q_STATE_DEF_INTERNAL(SPIMgr, idle) {
            QP::QState status_;

            switch (e->sig) {
                case Q_ENTRY_SIG:
                    // On entry to &idle, attempt to recall an event from the internal queue.  The event at the head of the
                    // internal queue will be posted LIFO of the AO queue (which is what we want).  This preserves the original
                    // order of the SPI_TRANSACTION_SIG events that were passed to SPIMgr.
                    recall(&SpiTransactionQueue);
                    status_ = Q_RET_HANDLED;
                    break;

                case Q_EXIT_SIG:
                    status_ = Q_RET_HANDLED;
                    break;

                    // If we get a transaction request, and we aren't in the &busy state, go ahead and begin the transaction.
                case SPI_traits::SPI_TRANSACTION_SIG:
                    setCurrentTransaction(static_cast<const TransactionEvent *>(e));
                    status_ = tran(&chip_select_setup_time);
                    break;

                default:
                    status_ = super(&top);
                    break;
            }

            return status_;
        }

        //! busy
        Q_STATE_DEF_INTERNAL(SPIMgr, busy) {
            QP::QState status_;

            switch (e->sig) {
                case Q_ENTRY_SIG:
                    status_ = Q_RET_HANDLED;
                    break;
                case Q_EXIT_SIG:
                    status_ = Q_RET_HANDLED;
                    break;

                    // If we get a transaction request, and we are in the &busy state, defer it to our internal QP queue.
                    // Once we are not busy, we will grab transactions from the internal queue first.
                case SPI_traits::SPI_TRANSACTION_SIG:
                    defer(&SpiTransactionQueue, e);
                    status_ = Q_RET_HANDLED;
                    break;

                default:
                    status_ = super(&top);
                    break;
            }

            return status_;
        }

        //! busy::chip_select_setup_time
        Q_STATE_DEF_INTERNAL(SPIMgr, chip_select_setup_time) {
            QP::QState status_;

            switch (e->sig) {
                case Q_ENTRY_SIG:
                    EnableCurrentTransactionChipSelect(true);
                    StartChipSelectTimer(CurrentTransaction.config.setup_time_us);
                    status_ = Q_RET_HANDLED;
                    break;
                case Q_EXIT_SIG:
                    status_ = Q_RET_HANDLED;
                    break;

                case TIMER_PERIOD_ELAPSED_SIG:
                    StopChipSelectTimer();
                    status_ = tran(&transaction_in_progress);
                    break;

                default:
                    status_ = super(&busy);
                    break;
            }

            return status_;
        }

        //! busy::transaction_in_progress
        Q_STATE_DEF_INTERNAL(SPIMgr, transaction_in_progress) {
            QP::QState status_;

            switch (e->sig) {
                case Q_ENTRY_SIG:
                    startTransaction();
                    status_ = Q_RET_HANDLED;
                    break;
                case Q_EXIT_SIG:
                    status_ = Q_RET_HANDLED;
                    break;

                case TRANSACTION_DONE_SIG:
                    TransactionDoneEvent();
                    status_ = tran(&chip_select_hold_time);
                    break;

                default:
                    status_ = super(&busy);
                    break;
            }

            return status_;
        }

        //! @todo Some debate as to whether this state is needed.
        //! busy::chip_select_hold_time
        Q_STATE_DEF_INTERNAL(SPIMgr, chip_select_hold_time) {
            QP::QState status_;

            switch (e->sig) {
                case Q_ENTRY_SIG:
                    StartChipSelectTimer(CurrentTransaction.config.hold_time_us);
                    status_ = Q_RET_HANDLED;
                    break;
                case Q_EXIT_SIG:
                    status_ = Q_RET_HANDLED;
                    break;

                case TIMER_PERIOD_ELAPSED_SIG:
                    StopChipSelectTimer();
                    EnableCurrentTransactionChipSelect(false);
                    status_ = tran(&idle);
                    break;

                default:
                    status_ = super(&busy);
                    break;
            }

            return status_;
        }
};

template<typename SPI_traits>
void SPIMgr<SPI_traits>::publishTransactionCompleteEvent(const enum_t qp_complete_signal,
                                                         const ErrorCode error_code,
                                                         const spi_buffer &read_buf,
                                                         const size_t transaction_len) {
    Q_ASSERT_ID(2, qp_complete_signal <= MAX_PUB_SIG);
    Q_ASSERT_ID(3, qp_complete_signal >= QP::Q_USER_SIG);

    TransactionCompleteEvent *e = Q_NEW(TransactionCompleteEvent, qp_complete_signal);
    e->read_buf = read_buf;
    e->transaction_len = transaction_len;
    e->error_code = error_code;
    QP::QF::PUBLISH(e, this);
}

template<typename SPI_traits>
void SPIMgr<SPI_traits>::ConfigureClockPolarity(const ClockPolarity clock_polarity) {
    switch (clock_polarity) {
        case ClockPolarity::Low:
            SPI_traits::spi_handle->Instance->CFG2 &= ~SPI_POLARITY_HIGH;
            break;
        case ClockPolarity::High:
            SPI_traits::spi_handle->Instance->CFG2 |= SPI_POLARITY_HIGH;
            break;
        default:
            Q_ASSERT_ID(25, false);
            break;
    }
}

template<typename SPI_traits>
void SPIMgr<SPI_traits>::ConfigureClockPhase(const ClockPhase clock_phase) {
    switch (clock_phase) {
        case ClockPhase::OneEdge:
            SPI_traits::spi_handle->Instance->CFG2 &= ~SPI_PHASE_2EDGE;
            break;
        case ClockPhase::TwoEdge:
            SPI_traits::spi_handle->Instance->CFG2 |= SPI_PHASE_2EDGE;
            break;
        default:
            Q_ASSERT_ID(30, false);
            break;
    }
}

template<typename SPI_traits>
void SPIMgr<SPI_traits>::setCurrentTransaction(const TransactionEvent * e) {
    Q_ASSERT_ID(5, e->transaction_len <= MaxSPITransactionLength);
    Q_ASSERT_ID(10, e->transaction_len != 0);
    Q_ASSERT_ID(15, e->config.setup_time_us <= MaximumAcceptableSetupTime_us);
    Q_ASSERT_ID(20, e->config.hold_time_us <= MaximumAcceptableHoldTime_us);

    //! @todo Call SPI translation layer driver (not created yet).
    // Set SPI clock polarity and clock phase.
    ConfigureClockPolarity(e->config.clock_polarity);
    ConfigureClockPhase(e->config.clock_phase);

    // Copy write data into static buffer for use by the transaction.
    CurrentTransaction.chip_select_pin = e->chip_select_pin;
    CurrentTransaction.qp_complete_signal = e->qp_complete_signal;
    CurrentTransaction.config = e->config;
    CurrentTransaction.write_buf = e->write_buf;
    CurrentTransaction.transaction_len = e->transaction_len;
}

template<typename SPI_traits>
void SPIMgr<SPI_traits>::EnableCurrentTransactionChipSelect(bool enable) {
    GPIO_TypeDef *gpio_reg = CurrentTransaction.chip_select_pin.reg;
    uint32_t gpio_pin = CurrentTransaction.chip_select_pin.pin;

    if (enable)
        HAL_GPIO_WritePin(gpio_reg, gpio_pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(gpio_reg, gpio_pin, GPIO_PIN_SET);
}

} // namespace SPI
} // namespace COMMON

#endif /* INCLUDE_SPIMGR_H_ */
