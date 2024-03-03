/**
 * @file DigitalInputMgr.h
 *
 * @author dsmoot
 * @author awong
 */

#ifndef SRC_DIGITALINPUTMGR_H_
#define SRC_DIGITALINPUTMGR_H_

#include "Project.h"
#include "qpcpp.hpp"
#include "qphelper.h"

#include "stm32h7xx.h"

#include "bsp.h"

namespace COMMON {
namespace DIGITAL_INPUT {

struct DInPin {
    GPIO_TypeDef * const Port; //! GPIO input pin port.
    uint16_t const PinNum; //! GPIO input pin mask.

    /** QP signal to be published upon change of digital input state.
     *  If QP publish is not desirable, set to 0 (i.e. you only want to print input changes
     *  using QSPY for troubleshooting or hardware checkout).
     */
    enum_t qp_complete_signal;

    /** Number of consecutive identical states required before considering the state to be changed.
     *  Note: Dependent on the PollPeriodMs assigned in the Traits.
     */
    size_t debounce_length;
};

//template<size_t num_digital_inputs>
//struct DigitalInputTraits {
//    static constexpr std::array<DInPin, num_polled_inputs> DInPinList{};
//
//    static constexpr uint32_t PollPeriodMs = 0;
//};

template<typename Traits>
class DigitalInputMgr : public QP::QActive {
private:
    enum InternalSignals {
        TIMEOUT_SIG = MAX_PUB_SIG,
        STARTUP_SIG
    };

    static constexpr size_t PollPeriodTicks = Traits::PollPeriodMs / COMMON::BSP::MsPerBspTick;

public:
    DigitalInputMgr()
    : QActive(Q_STATE_CAST(&DigitalInputMgr::initial)),
      PollTimer(this, TIMEOUT_SIG),
      pin_state_list{}
    {
        ;
    }

private:
    QP::QTimeEvt PollTimer;

    struct PinState {
        bool last_state;
        uint32_t stable_count;
        bool last_stable_state;
        bool first_publish_has_occurred;
    };

    std::array<PinState, Traits::DInPinList.size()> pin_state_list;

    void PublishDigitalInputStateChange(const bool state, const enum_t qp_complete_signal) {
        BoolEvt * Bevt = Q_NEW(BoolEvt, qp_complete_signal);
        Bevt->data = state;
        QP::QF::PUBLISH(Bevt, this);
    }

    /*
     * Nested loops of all pins and ports.  Once a match of ports, look for a particular bit change with
     */
    void PollForChangesAndDebounce() {
        for (size_t pin_index = 0; pin_index < num_polled_inputs; pin_index++) {
            auto& ppc = Traits::DInPinList[pin_index];
            auto &pin_state = pin_state_list[pin_index];

            bool current_state = ppc.Port->IDR & ppc.PinNum;
            if (current_state != pin_state.last_state) {
                pin_state.stable_count = 0;
            }
            else if (pin_state.stable_count < ppc.debounce_length) {
                pin_state.stable_count++;
            }
            // If we've exceeded desired stable count for debouncing, only publish when the last stable state changes.
            else if ((current_state != pin_state.last_stable_state)
                  || !pin_state.first_publish_has_occurred) {
                pin_state.first_publish_has_occurred = true;

                // Only publish a signal if desired by application.
                if (ppc.qp_complete_signal != 0) {
                    PublishDigitalInputStateChange(current_state, ppc.qp_complete_signal);
                }

                QS_BEGIN_ID(DigitalInputMsg, getPrio())
                    QS_STR("Digital In chan:");
                    QS_U8(0, pin_index);
                    QS_STR(current_state ? "went high" : "went low");
                QS_END()

                pin_state.last_stable_state = current_state;
            }

            pin_state.last_state = current_state;
        }
    }


protected:
    Q_STATE_DEF_INTERNAL(DigitalInputMgr, initial) {
        (void) e;

        return tran(&running);
    }

    Q_STATE_DEF_INTERNAL(DigitalInputMgr, running) {
        QP::QState status_;

        switch(e->sig) {
            case Q_ENTRY_SIG:
                PollTimer.armX(PollPeriodTicks, PollPeriodTicks);
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                PollTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            case TIMEOUT_SIG:
                PollForChangesAndDebounce();
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }
};
} /* namespace DIGITAL_INPUT */
} /* namespace COMMON */

#endif /* SRC_DIGITALINPUTMGR_H_ */
