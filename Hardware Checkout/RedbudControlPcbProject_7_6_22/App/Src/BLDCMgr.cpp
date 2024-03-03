/**
 * @file BLDCMgr.h
 * @date 2022/04/07
 * @author rsutton
 *
 * AO designed for the MDC010-024031 brushless DC motor controller.
 */
#include <cstdint>
#include "qpcpp.hpp"
#include "Project.h"
#include "stm32h7xx_hal_tim.h"
#include "tim.h"
#include "BLDCMgr.h"
#include "main.h"

BLDC::BLDCMgr BLDC::BLDCMgr::inst;
QP::QActive * const AO_BLDCMgr = &BLDC::BLDCMgr::inst;

namespace BLDC {

Q_DEFINE_THIS_MODULE("BLDCMgr")

enum internalSignals {
    TIMEOUT_SIG = MAX_PUB_SIG
};

BLDCMgr::BLDCMgr()
: QActive(&initial),
  BLDCStatusTimer(this, TIMEOUT_SIG, 0U),
  is_faulted(true)
{}

Q_STATE_DEF(BLDCMgr, initial) {
    (void)e;

    // Pull enable low before configuration of IN pins.
    HAL_GPIO_WritePin(BLDC_EN_GPIO_Port, BLDC_EN_Pin, GPIO_PIN_RESET);

    // This configures for open loop PWM control.
    // TODO: Allow configuration?
    HAL_GPIO_WritePin(BLDC_IN1_GPIO_Port, BLDC_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BLDC_IN2_GPIO_Port, BLDC_IN2_Pin, GPIO_PIN_RESET);

    // TODO: Some required delay between above steps and enable?

    // After config of IN pins, set EN high.
    HAL_GPIO_WritePin(BLDC_EN_GPIO_Port, BLDC_EN_Pin, GPIO_PIN_SET);

    subscribe(SET_BLDC_SPEED_PERCENTAGE_SIG);

    return tran(&running);
}

Q_STATE_DEF(BLDCMgr, running) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BLDCStatusTimer.armX(BLDCStatusPeriodTicks, BLDCStatusPeriodTicks);
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            BLDCStatusTimer.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        case SET_BLDC_SPEED_PERCENTAGE_SIG: {
            handleSetBLDCSpeedPercentage(static_cast<const SetBLDCSpeedPercentageEvent *>(e));
            status_ = Q_RET_HANDLED;
            break;
        }
        case TIMEOUT_SIG: {
            // This can mean:
            // - Undervoltage
            // - Overvoltage
            // - Thermal Overload
            // - Invalid Hall Sensor Signals
            is_faulted = !HAL_GPIO_ReadPin(BLDC_STATUS_GPIO_Port, BLDC_STATUS_Pin);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

void BLDCMgr::handleSetBLDCSpeedPercentage(const SetBLDCSpeedPercentageEvent * e) {
    // TODO: Document direction from bool.
    HAL_GPIO_WritePin(BLDC_DIR_GPIO_Port, BLDC_DIR_Pin, e->direction);
    setVoltageOutput(e->speed);
}

void BLDCMgr::setVoltageOutput(percentage setpoint) {
    writePulseCountToTimer(
            percentageToPulseCount(setpoint));
}

pulse_count BLDCMgr::percentageToPulseCount(percentage input) {
    percentage saturated_input;
    if (input > 100u)
        saturated_input = 100;
    else
        saturated_input = input;
    return static_cast<pulse_count>(saturated_input * 655.35f);
}

void BLDCMgr::writePulseCountToTimer(pulse_count setpoint) {
    ///@todo UNHANDLED ERRORS
    HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1);

    TIM_OC_InitTypeDef new_config {};

    new_config.OCMode = TIM_OCMODE_PWM1;
    new_config.Pulse = setpoint;
    new_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    new_config.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim13, &new_config, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
}

} // BLDC
