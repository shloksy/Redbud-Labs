/**
 * @file TMC429Homing.h
 *
 * @author awong
 *
 * Helper class that deals with state information related with homing transactions.  Used
 * directly by TMC429Mgr.
 */

#ifndef APP_COMMON_INCLUDE_TMC429_TMC429HOMING_H_
#define APP_COMMON_INCLUDE_TMC429_TMC429HOMING_H_

#include "TMC429Protocol.h"

#include <stddef.h>
#include <array>

namespace COMMON {
namespace TMC429 {

/**
 * Helper struct that manages state of homing (for TMC429Mgr).
 */
struct HomingState {
    size_t homing_commands_sent; //! Number of homing commands sent.
    size_t homing_command_responses_received; //! Number of homing command responses received.
    size_t zero_commands_sent; //! Number of zero commands sent.
    size_t zero_command_response_received; //! Number of zero command responses sent.

    std::array<bool, NumStepperMotors> homing_enabled; //! Configured homing enabled for each motor.
    std::array<int16_t, NumStepperMotors> homing_velocity; //! Configured homing velocity.

    //! Constructor which sets the initial state of HomingState.
    HomingState() {
        reset();
    }

    //! Resets internal state.
    void reset() {
        homing_commands_sent = 0;
        homing_command_responses_received = 0;
        zero_commands_sent = 0;
        zero_command_response_received = 0;

        homing_enabled.fill(0);
    }

    //! Configures which motors will be homed.
    void set_motors_homed(const std::array<bool, NumStepperMotors> &motors_homed) {
        homing_enabled = motors_homed;
    }

    //! Configures each motors' homing velocity.
    void set_motors_homing_velocity(const std::array<int16_t, NumStepperMotors> &motor_homing_velocity) {
        homing_velocity = motor_homing_velocity;
    }

    //! Collects the command response and increments an internal counter.
    void collect_command_response() {
        // Don't do anything with the responses on command response but increment.
        homing_command_responses_received++;
    }

    //! Returns whether the command is complete or not.
    bool is_command_complete() const {
        return homing_command_responses_received >= homing_commands_sent;
    }

    //! Returns whether all homed motors have hit their reference switches.
    bool switches_hit(StatusBits &status) const {
        // Start in triggered state, and clear if we don't detect triggered switch.
        bool all_switches_triggered = true;
        for (auto motor_index = 0; motor_index < NumStepperMotors; motor_index++) {
            all_switches_triggered &= !homing_enabled[motor_index] || status.RS[motor_index];
        }

        return all_switches_triggered;
    }

    //! Collects the zero response and increments an internal counter.
    void collect_zero_response() {
        // Don't do anything with the responses on zero response but increment.
        zero_command_response_received++;
    }

    //! Returns whether zeroing has completed or not.
    bool is_zeroing_complete() const {
        return zero_command_response_received >= zero_commands_sent;
    }
};

} // namespace TMC429
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_TMC429_TMC429HOMING_H_ */
