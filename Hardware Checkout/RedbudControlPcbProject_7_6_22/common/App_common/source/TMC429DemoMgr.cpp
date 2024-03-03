/**
 * @file TMC429DemoMgr.cpp
 *
 * @author awong
 *
 * Example AO that demonstrates a typical use-case of the TMC429Mgr interface.
 */

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"

#include "TMC429DemoMgr.h"

Q_DEFINE_THIS_MODULE("TMC429DemoMgr")

namespace COMMON {

    void TMC429DemoMgr::ConfigureAllMotorDrivers() const {
        for (size_t motor_index = TMC429::StepperMotor1;
             motor_index < TMC429::NumStepperMotors;
             motor_index++) {
            PublishTMC429ConfigEvent(static_cast<TMC429::StepperMotorIndex>(motor_index));
            PublishTMC429SetCurrentScalingEvent(static_cast<TMC429::StepperMotorIndex>(motor_index));
        }
    }

    void TMC429DemoMgr::PublishTMC429ConfigEvent(const TMC429::StepperMotorIndex motor_index) const {
        auto e = Q_NEW(TMC429::ConfigMotorEvent, TMC429_CONFIG_MOTOR_SIG);
        e->motor_index = motor_index;

        // TODO: For now, just copy default configuration from the EVAL example software.
        e->config.acceleration_max = 25;
        e->config.pulse_div = 3;
        e->config.ramp_div = 7;
        e->config.ramp_mode = TMC429::VelocityMode;
        e->config.ustep_resolution_selection = TMC429::Microsteps64;
        e->config.velocity_max = 500;
        e->config.velocity_min = 1;

        QP::QF::PUBLISH(e, this);
    }

    void TMC429DemoMgr::PublishTMC429SetCurrentScalingEvent(const TMC429::StepperMotorIndex motor_index) const {
        auto e = Q_NEW(TMC429::SetCurrentScalingEvent, TMC429_SET_CURRENT_SCALING_SIG);
        e->motor_index = motor_index;

        // TODO: For now, just use hard-coded values.
        e->is_agtat = TMC429::IScale100;
        e->is_aleat = TMC429::IScale100;
        e->is_v0 = TMC429::IScale12_5; // Scales V0 (when at target position) to 12.5% of max.
        e->a_threshold = 50;

        QP::QF::PUBLISH(e, this);
    }

    void TMC429DemoMgr::PublishTMC429HomingEvent() const {
        auto e = Q_NEW(TMC429::HomingCommandEvent, TMC429_HOMING_COMMAND_SIG);
        e->homing_enabled = {true, true, true};
        e->homing_velocity = {-25, -25, -25};
        QP::QF::PUBLISH(e, this);
    }

    void TMC429DemoMgr::PublishTMC429QueryMotorFeedback(const TMC429::StepperMotorIndex motor_index) const {
        auto e = Q_NEW(TMC429::QueryMotorFeedbackEvent, TMC429_QUERY_MOTOR_FEEDBACK_SIG);
        e->motor_index = motor_index;
        QP::QF::PUBLISH(e, this);
    }

    Q_STATE_DEF(TMC429DemoMgr, initial) {
        (void)e;

        subscribe(TMC429_HOMING_COMMAND_COMPLETE_SIG);
        subscribe(TMC429_MOTOR_FEEDBACK_SIG);
        subscribe(QVIEW_MOTOR_HOME_SIG);

        return tran(&startup);
    }

    Q_STATE_DEF(TMC429DemoMgr, startup) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                PostStartupStart();
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                status_ = Q_RET_HANDLED;
                break;

            case START_OPERATION_SIG:
                ConfigureAllMotorDrivers();
#if CONFIG_ENABLE_TMC429_HOMING_ON_INITIALIZATION
                PublishTMC429HomingEvent();
                status_ = Q_RET_HANDLED;
#else
                status_ = tran(&idle);
#endif
                break;

            case TMC429_HOMING_COMMAND_COMPLETE_SIG:
                status_ = tran(&idle);
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

    Q_STATE_DEF(TMC429DemoMgr, idle) {
        QP::QState status_;

        switch (e->sig) {
            case Q_ENTRY_SIG:
                QueryMotorFeedbackTimer.armX(QueryMotorFeedbackPeriodTicks, QueryMotorFeedbackPeriodTicks);
                status_ = Q_RET_HANDLED;
                break;
            case Q_EXIT_SIG:
                QueryMotorFeedbackTimer.disarm();
                status_ = Q_RET_HANDLED;
                break;

            case QUERY_MOTOR_FEEDBACK_PERIODIC_SIG:
                PublishTMC429QueryMotorFeedback(TMC429::StepperMotor1);
                PublishTMC429QueryMotorFeedback(TMC429::StepperMotor2);
                PublishTMC429QueryMotorFeedback(TMC429::StepperMotor3);
                status_ = Q_RET_HANDLED;
                break;

            case TMC429_MOTOR_FEEDBACK_SIG:
                // TODO: Application can handle motor feedback signal here if desired.
                status_ = Q_RET_HANDLED;
                break;

            case QVIEW_MOTOR_HOME_SIG:
                PublishTMC429HomingEvent();
                status_ = Q_RET_HANDLED;
                break;

            default:
                status_ = super(&top);
                break;
        }

        return status_;
    }

} // namespace COMMON
