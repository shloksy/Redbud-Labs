/*
 * AdcManager.h
 *
 *  Created on: May 24, 2021
 *      Author: dsmoot
 */

#ifndef SRC_ADCMANAGER_H_
#define SRC_ADCMANAGER_H_

#include "qpcpp.h"
#include "Project.h"
#include <array>
#include "stm32h7xx_hal_adc.h"
#include "AnalogMgrApp.h"

namespace COMMON {
namespace ANALOG_INPUT {
class AnalogMgr: public QP::QActive {
private:
    class ConversionCompleteEvt : public QP::QEvt {
    public:
        AdcPeripheralIndex adc_peripheral_index;
    };

    QP::QTimeEvt SampleTimer;

    // TODO: Should this be volatile??
    std::array<uint32_t, NumAdcChannels> DmaCache;

    std::array<ADC_HandleTypeDef *, NumAdcPeripheralsUsed> pAdcHandleList;
    std::array<bool, NumAdcPeripheralsUsed> AdcConversionComplete;

public:
    AnalogMgr();
    void DmaCompleteCallback(ADC_HandleTypeDef * pAdcHandle);
    void SetpAdcHandle(AdcPeripheralIndex adc_peripheral_index, ADC_HandleTypeDef * val);

private:
    QP::QState handleInternalConversionCompleteEvent(const ConversionCompleteEvt * e);
    void PostAdcConversionCompleteEvent(AdcPeripheralIndex adc_peripheral_index);
    void PublishAdcDataEvent() const;
    bool AllPeripheralConversionsComplete() const;
    void ResetPeripheralConversionCompleteFlags();
    void QspyAdcSamplesMessage();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(running);
};

} /*namespace ANALOG_INPUT */
} /*namespace COMMON */
#endif /* SRC_ADCMANAGER_H_ */
