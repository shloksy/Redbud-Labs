/**
 * @file AdcManager.cpp
 *
 * @author dsmoot
 */

#include "AnalogMgr.h"
#include "bsp.h"
#include "string.h"
#include "Project.h"
#include "AnalogMgrApp.h"

using namespace COMMON::ANALOG_INPUT;

static AnalogMgr l_AnlgMgr;
QP::QActive * const AO_AnlgMgr = &l_AnlgMgr;

namespace COMMON {
namespace ANALOG_INPUT {
Q_DEFINE_THIS_MODULE("AnalogMgr")

enum InternalSignals {
    TIMEOUT_SIG = LAST_COMMON_SIG,
    INTERNAL_CONVERSION_COMPLETE
};



extern "C" {

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

    /*
     * Being lazy for now and not checking the argument.
     * Also being lazy and not checking the error case
     */
    l_AnlgMgr.DmaCompleteCallback(hadc);
}
}

AnalogMgr::AnalogMgr()
: QActive(Q_STATE_CAST(&AnalogMgr::initial)),
  SampleTimer(this, TIMEOUT_SIG),
  DmaCache{0},
  pAdcHandleList{nullptr},
  // For now, prime the conversion so that first AllPeripheralConversionsComplete() check passes.
  AdcConversionComplete{true}
  {
      for (int per_index = 0; per_index < NumAdcPeripheralsUsed; per_index++) {
          AdcConversionComplete[per_index] = true;
      }

  }

  Q_STATE_DEF(AnalogMgr, initial) {
      (void) e;

      uint32_t TimerTicks = AdcSamplePeriodMs * BSP::BSP_TICKS_PER_SEC / 1000;

      SampleTimer.armX(TimerTicks, TimerTicks);

      return tran(&running);
  }

  Q_STATE_DEF(AnalogMgr, running) {
      QP::QState status_;

      switch(e->sig) {
          case Q_EXIT_SIG:
              SampleTimer.disarm();
              status_ = Q_RET_HANDLED;
              break;

          case Q_ENTRY_SIG:
              status_ = Q_RET_HANDLED;
              break;

              /*
               * Pretty simple, trigger a conversion on every timer expiration
               */
          case TIMEOUT_SIG:
              if (!AllPeripheralConversionsComplete()) {
                  // TODO: If all peripherals haven't completed conversion, something went really wrong.
              }
              else {
                  ResetPeripheralConversionCompleteFlags();
                  for (std::size_t adc_peripheral_index = 0; adc_peripheral_index < NumAdcPeripheralsUsed; adc_peripheral_index++) {
                      if (pAdcHandleList[adc_peripheral_index] != nullptr) {
                          if (HAL_OK != HAL_ADC_Start_DMA(pAdcHandleList[adc_peripheral_index],
                                  DmaCache.data() + AdcLogicalConfig[adc_peripheral_index].FirstChannel,
                                  AdcLogicalConfig[adc_peripheral_index].NumberOfChannels)) {
                              // TODO: asw  Assume no error here for now.  This becomes slightly more complicated with multiple
                              // ADC peripherals.

                              //this ensures we get an event for every attempted sample even if DMA fails.
                              //this way data consumers don't have to worry about gaps in the event stream.
                              //but they do have to worry about garbage data.
                              //COMMON::AdcDataEvt * Evt = Q_NEW(COMMON::AdcDataEvt, COMMON::ADC_DATA_SIG);
                              //memset(Evt->samples, 0, sizeof(Evt->samples));
                              //QP::QF::PUBLISH(Evt, this);
                          }
                      }
                  }
              }

              status_ = Q_RET_HANDLED;
              break;

          case INTERNAL_CONVERSION_COMPLETE:
              status_ = handleInternalConversionCompleteEvent(static_cast<const ConversionCompleteEvt *>(e));
              break;

          default:
              status_ = super(&top);
              break;
      }
      return status_;
  }

  bool AnalogMgr::AllPeripheralConversionsComplete() const {
      bool conv_complete_flag = true;
      for (int per_index = 0; per_index < NumAdcPeripheralsUsed; per_index++) {
          conv_complete_flag &= AdcConversionComplete[per_index];
      }

      return conv_complete_flag;
  }

  void AnalogMgr::ResetPeripheralConversionCompleteFlags() {
      for (int per_index = 0; per_index < NumAdcPeripheralsUsed; per_index++) {
          AdcConversionComplete[per_index] = false;
      }
  }

  void AnalogMgr::QspyAdcSamplesMessage() {
      QS_BEGIN_ID(ADCMsg, this->m_prio)
            for (int adc_index = 0; adc_index < NumAdcChannels; adc_index++) {
                QS_U32_HEX(4, DmaCache[adc_index]);
            }
      QS_END()

  }

  QP::QState AnalogMgr::handleInternalConversionCompleteEvent(const ConversionCompleteEvt * e) {
      Q_ASSERT(e->adc_peripheral_index < NumAdcPeripheralsUsed &&
              e->adc_peripheral_index >= 0);

      AdcConversionComplete[e->adc_peripheral_index] = true;

      if (AllPeripheralConversionsComplete()) {
          PublishAdcDataEvent();

          QspyAdcSamplesMessage();
      }

      return Q_RET_HANDLED;
  }


  void AnalogMgr::SetpAdcHandle(AdcPeripheralIndex adc_peripheral_index, ADC_HandleTypeDef * val) {
      Q_ASSERT(adc_peripheral_index >= 0 &&
              adc_peripheral_index < NumAdcPeripheralsUsed);

      pAdcHandleList[adc_peripheral_index] = val;
  }

  void AnalogMgr::DmaCompleteCallback(ADC_HandleTypeDef * pAdcHandle) {
      for (std::size_t adc_peripheral_index = 0; adc_peripheral_index < NumAdcPeripheralsUsed; adc_peripheral_index++) {
          if (pAdcHandle == pAdcHandleList[adc_peripheral_index] &&
                  pAdcHandle != nullptr) {
              PostAdcConversionCompleteEvent(static_cast<AdcPeripheralIndex>(adc_peripheral_index));

              HAL_ADC_Stop_DMA(pAdcHandle);
          }
      }
  }

  void AnalogMgr::PostAdcConversionCompleteEvent(AdcPeripheralIndex adc_peripheral_index) {
      auto e = Q_NEW(ConversionCompleteEvt, INTERNAL_CONVERSION_COMPLETE);
      e->adc_peripheral_index = adc_peripheral_index;
      POST(e, this);
  }

  void AnalogMgr::PublishAdcDataEvent() const {
      auto Evt = Q_NEW(AdcDataEvt, ADC_DATA_SIG);

      for (std::size_t adc_channel = 0; adc_channel < NumAdcChannels; adc_channel++) {
          Evt->samples[adc_channel] = DmaCache[adc_channel] & 0xFFFF;
      }

      QP::QF::PUBLISH(Evt, this);
  }

} /*namespace ANALOG_INPUT */
} /*namespace COMMON */
