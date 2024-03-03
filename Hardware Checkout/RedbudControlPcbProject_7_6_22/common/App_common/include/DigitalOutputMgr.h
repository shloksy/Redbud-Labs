/**
 * @file DigitalOutputMgr.h
 *
 * @author dsmoot
 * @author awong
 */

#ifndef SOURCE_DIGITALOUTPUTMGR_H_
#define SOURCE_DIGITALOUTPUTMGR_H_

#include "qpcpp.h"

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

#include "qphelper.h"

#include "QPEvt/DigitalOutputEvt.h"

#include <array>

namespace COMMON {
namespace DIGITAL_OUTPUT {

struct DOutPin {
    GPIO_TypeDef * const Port;
    uint16_t const PinNum;
};

//template<size_t num_digital_outputs>
//struct DigitalOutputTraits {
//    static constexpr enum_t DIGITAL_OUTPUT_SIG = 0;
//
//    static constexpr std::array<DOutPin, num_digital_outputs> DOutPinList{};
//};

template<typename Traits>
class DigitalOutputMgr: public QP::QActive, Traits {
public:
    DigitalOutputMgr()
    : QActive(Q_STATE_CAST(&DigitalOutputMgr::initial)){
        ;
    }

private:
    void QspyDigitalOutputMessage(int index, bool state) {
        QS_BEGIN_ID(DigitalOutputMsg, getPrio())
            QS_STR("Digital Out chan:");
            QS_U8(0, index);
            QS_STR(state ? "went high" : "went low");
        QS_END()
    }

    void handleDigitalOutputEvent(const DigitalOutputEvent * e) {
        const auto digital_out_index = e->digital_output_index;
        // Ignore indices that aren't within bounds.
        if (digital_out_index >= Traits::DOutPinList.size()) {
            QS_BEGIN_ID(DigitalOutputMsg, getPrio())
                QS_STR("ERROR: Invalid digital output index=");
                QS_U8(0, digital_out_index);
            QS_END()
        }
        else {
            const auto port = Traits::DOutPinList[digital_out_index].Port;
            const auto PinNum = Traits::DOutPinList[digital_out_index].PinNum;
            HAL_GPIO_WritePin(port,
                              PinNum,
                              e->enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
            QspyDigitalOutputMessage(digital_out_index, e->enable);
        }

    }

protected:
    Q_STATE_DEF_INTERNAL(DigitalOutputMgr, initial) {
        (void) e;

        subscribe(Traits::DIGITAL_OUTPUT_SIG);

        return tran(&running);
    }

    Q_STATE_DEF_INTERNAL(DigitalOutputMgr, running) {
        QP::QState status_;

        switch(e->sig) {
            case Q_ENTRY_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case Traits::DIGITAL_OUTPUT_SIG:
                handleDigitalOutputEvent(static_cast<const DigitalOutputEvent *>(e));
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&top);
        }

        return status_;
    }
};
} /* namespace DIGITAL_OUTPUT */
} /* namespace COMMON */

#endif /* SOURCE_DIGITALOUTPUTMGR_H_ */
