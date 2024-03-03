/**
 * @file TMC429DemoMgr.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_TMC429DEMOMGR_H_
#define APP_COMMON_INCLUDE_TMC429DEMOMGR_H_

#include <stdint.h>

#include "Project.h"
#include "qpcpp.hpp"
#include "bsp.h"

#include "TMC429Protocol.h"

namespace COMMON {

class TMC429DemoMgr : public QP::QActive {
private:
    enum InternalSignals {
        START_OPERATION_SIG = MAX_PUB_SIG,
        QUERY_MOTOR_FEEDBACK_PERIODIC_SIG,
    };

    static constexpr uint32_t QueryMotorFeedbackPeriodMs = 500;
    static constexpr uint32_t QueryMotorFeedbackPeriodTicks = QueryMotorFeedbackPeriodMs / COMMON::BSP::MsPerBspTick;

    QP::QTimeEvt QueryMotorFeedbackTimer;

public:
    TMC429DemoMgr() :
        QActive(Q_STATE_CAST(&TMC429DemoMgr::initial)),
        QueryMotorFeedbackTimer(this, QUERY_MOTOR_FEEDBACK_PERIODIC_SIG, 0U)
    {
        ;
    }

private:
    void PostStartupStart() {
        POST(Q_NEW(QP::QEvt, START_OPERATION_SIG), this);
    }
    void ConfigureAllMotorDrivers() const;
    void PublishTMC429ConfigEvent(const TMC429::StepperMotorIndex motor_index) const;
    void PublishTMC429SetCurrentScalingEvent(const TMC429::StepperMotorIndex motor_index) const;
    void PublishTMC429HomingEvent() const;
    void PublishTMC429QueryMotorFeedback(const TMC429::StepperMotorIndex motor_index) const;

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(startup);
    Q_STATE_DECL(idle);
};

} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_TMC429DEMOMGR_H_ */
