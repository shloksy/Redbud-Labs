/**
 * @file TMC429Protocol.cpp
 *
 * @author awong
 *
 * Contains helper functions to build and parse transactions to the TMC429 stepper motor controller.
 * This is pulled directly from the TMC429 datasheet.
 *
 * TODO: Only functions used by the application were implemented, so there are a decent amount
 *       of holes here.
 * TODO: Several transactions should include more parameters, but don't yet.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "TMC429Protocol.h"

#include <array>

Q_DEFINE_THIS_MODULE("TMC429Protocol.cpp")

namespace COMMON {
namespace TMC429 {

// Use case #1: Accessing a specific stepper motor register.
constexpr uint8_t ControlByte::pack(const ReadOrWrite read_or_write,
                                    const IndividualMotorRegisters indiv_motor_reg,
                                    const StepperMotorIndex motor_index) {
    Q_ASSERT_ID(1, motor_index >= StepperMotor1
                && motor_index < NumStepperMotors);
    return pack_common(read_or_write, static_cast<uint8_t>(indiv_motor_reg), static_cast<uint8_t>(motor_index));
}

namespace build_transaction {

// TODO: Could be generic template and moved to common helper.
// Used to convert int16 to int12 before sending to TMC429 for a handful of register writes.
static uint16_t int16_to_int12(const int16_t val) {
    return (val & 0x07FF) | ((val & 0x8000u) >> 4);
}

tmc429_trans_buf read_version() {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Read, TYPE_VERSION_429_REG);
    return cmd;
}

tmc429_trans_buf set_position_target(const StepperMotorIndex motor_index,
                                     const uint32_t x_target) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               X_TARGET_REG,
                               motor_index);
    cmd[1] = x_target >> 16;
    cmd[2] = x_target >> 8;
    cmd[3] = x_target >> 0;
    return cmd;
}

tmc429_trans_buf set_position_actual(const StepperMotorIndex motor_index,
                                     const uint32_t x_actual) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               X_ACTUAL_REG,
                               motor_index);
    cmd[1] = x_actual >> 16;
    cmd[2] = x_actual >> 8;
    cmd[3] = x_actual >> 0;
    return cmd;
}

tmc429_trans_buf get_position_actual(const StepperMotorIndex motor_index) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Read,
                               X_ACTUAL_REG,
                               motor_index);
    return cmd;
}

tmc429_trans_buf set_velocity_target(const StepperMotorIndex motor_index,
                                     const int16_t v_target) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               V_TARGET_REG,
                               motor_index);
    const auto v_target_actual = int16_to_int12(v_target);
    cmd[2] = v_target_actual >> 8;
    cmd[3] = v_target_actual >> 0;
    return cmd;
}

tmc429_trans_buf get_velocity_actual(const StepperMotorIndex motor_index) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Read,
                               V_ACTUAL_REG,
                               motor_index);
    return cmd;
}

tmc429_trans_buf set_current_scaling(const StepperMotorIndex motor_index,
                                     const IScalePercent i_scale_when_a_is_gt_a_threshold, // IS_AGTAT
                                     const IScalePercent i_scale_when_a_is_lt_a_threshold, // IS_ALEAT
                                     const IScalePercent i_scale_when_velocity_is_0, // IS_V0
                                     const uint16_t a_threshold) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               CURRENT_SCALING_VALUES_REG,
                               motor_index);
    const uint16_t a_threshold_actual = a_threshold & 0x7FF;
    cmd[1] = (i_scale_when_a_is_gt_a_threshold & 0x7) << 4
            | (i_scale_when_a_is_lt_a_threshold & 0x7) << 0;
    cmd[2] = (i_scale_when_velocity_is_0 & 0x7) << 4
            | (a_threshold_actual >> 8);
    cmd[3] = a_threshold_actual >> 0;
    return cmd;
}

//! @todo Generalize.
tmc429_trans_buf set_interrupt_mask(const StepperMotorIndex motor_index,
                                    const uint8_t interrupt_mask) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               INTERRUPT_REG,
                               motor_index);
    cmd[2] = interrupt_mask;
    return cmd;
}

tmc429_trans_buf get_interrupt_flags(const StepperMotorIndex motor_index) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Read,
                               INTERRUPT_REG,
                               motor_index);
    return cmd;
}

tmc429_trans_buf set_velocity_min(const StepperMotorIndex motor_index,
                                  const uint16_t v_min) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               V_MIN_REG,
                               motor_index);
    const uint16_t v_min_actual = v_min & 0x7FF; // V_MIN is 11 bits.
    cmd[2] = v_min_actual >> 8;
    cmd[3] = v_min_actual >> 0;
    return cmd;
}

tmc429_trans_buf set_velocity_max(const StepperMotorIndex motor_index,
                                  const uint16_t v_max) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               V_MAX_REG,
                               motor_index);
    const uint16_t v_max_actual = v_max & 0x7FF; // V_MAX is 11 bits.
    cmd[2] = v_max_actual >> 8;
    cmd[3] = v_max_actual >> 0;
    return cmd;
}

tmc429_trans_buf set_accel_max(const StepperMotorIndex motor_index,
                               const uint16_t a_max) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               A_MAX_REG,
                               motor_index);
    const uint16_t a_max_actual = a_max & 0x7FF; // A_MAX is 11 bits.
    cmd[2] = a_max_actual >> 8;
    cmd[3] = a_max_actual >> 0;
    return cmd;
}

tmc429_trans_buf set_deceleration_ramp(const StepperMotorIndex motor_index,
                                       const uint8_t pmul,
                                       const uint8_t pdiv) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               DECELERATION_RAMP_REG,
                               motor_index);
    const uint8_t pmul_actual = pmul & 0x7F; // PMUL is 7 bits.
    const uint8_t pdiv_actual = pdiv & 0xF; // PDIV is 4 bits.
    cmd[2] = pmul_actual | 0x80; // Always a 1 in bit 15 according to data sheet.
    cmd[3] = pdiv_actual;
    return cmd;
}

tmc429_trans_buf set_config_mode(const StepperMotorIndex motor_index,
                                 const MotionModes ramp_mode,
                                 const uint8_t ref_conf) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               CONFIG_MODE_REG,
                               motor_index);
    const uint8_t ref_conf_actual = ref_conf & 0xF; // REF_CONF is 4 bits.
    cmd[2] = ref_conf_actual;
    cmd[3] = ramp_mode;
    return cmd;
}

bool is_read_version_correct(const tmc429_trans_buf &read_buf) {
    return (read_buf[1] == TYPE_VERSION_429[0]) &&
            (read_buf[2] == TYPE_VERSION_429[1]) &&
            (read_buf[3] == TYPE_VERSION_429[2]);
}

tmc429_trans_buf set_accel_param_and_step_rate(const StepperMotorIndex motor_index,
                                               const uint8_t pulse_div,
                                               const uint8_t ramp_div,
                                               const MicrostepResolutionSelection usrs) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               ACCEL_PARAM_AND_STEP_RATE_REG,
                               motor_index);
    const uint8_t pulse_div_actual = pulse_div & 0xF; // PULSE_DIV is 4 bits.
    const uint8_t ramp_div_actual = ramp_div & 0xF; // RAMP_DIV is 4 bits.
    cmd[2] = (pulse_div_actual << 4) |
            (ramp_div_actual  << 0);
    cmd[3] = usrs;
    return cmd;
}

tmc429_trans_buf get_reference_switches() {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Read, REFERENCE_SWITCHES_REG);
    return cmd;
}

// TODO: This could take more parameters, but we're not using them right now.
tmc429_trans_buf set_global_parameters(const bool continuous_update,
                                       const uint8_t clk2_div,
                                       const LSMD_NumberMotorDrivers lsmd) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               STEPPER_MOTOR_GLOBAL_PARAMETER_REG);
    cmd[1] = continuous_update;
    cmd[2] = clk2_div;
    cmd[3] = lsmd;
    return cmd;
}

tmc429_trans_buf set_if_configuration(IfConfiguration config) {
    tmc429_trans_buf cmd = {};
    cmd[0] = ControlByte::pack(Write,
                               IF_CONFIGURATION_429_REG);
    cmd[2] = config.en_refr;
    cmd[3] = config.pack();
    return cmd;
}

tmc429_trans_buf set_driver_chain_datagram(const uint8_t datagram_offset,
                                           const uint8_t signal_code0,
                                           const uint8_t signal_code1) {
    // TODO: Q_ASSERT on offset greater than 32.

    tmc429_trans_buf cmd = {};
    // TODO: Magic number 0x80.
    // TODO: Write transaction should probably be explicit here (bit 0 of byte 0 is 0 on write).
    cmd[0] = 0x80 | ((datagram_offset & 0x1F) << 1);
    cmd[1] = signal_code0;
    cmd[2] = signal_code1;

    return cmd;
}

// TODO: Not recommended, but useful for testing.
tmc429_trans_buf set_general_command(const uint8_t address,
                                     const uint8_t byte0,
                                     const uint8_t byte1,
                                     const uint8_t byte2) {
    tmc429_trans_buf cmd = {};
    cmd[0] = address;
    cmd[1] = byte0;
    cmd[2] = byte1;
    cmd[3] = byte2;
    return cmd;
}

} // namespace build_transaction

namespace parse_transaction {

// TODO: Should be generic template and moved to common helper! The common helper should
// check whether we have arithmetic right shift support and choose between the two
// choices below.
// Used to convert int12 to int16 when parsing register reads.
// Note: This assumes that we have arithmetic right shift.  If not, then
// use:
//constexpr int16_t int12_to_int16(const uint16_t val)
//{
//   return (int(val + 0x0800) & 0x0FFF) - 0x0800;
//}
static int16_t int12_to_int16(const uint16_t val) {
    static_assert((int16_t(-1) >> 4) == -1);

    return int16_t(val << 4) >> 4;
}

uint32_t get_position_from_response(const tmc429_trans_buf &buf) {
    return buf[1] << 16
         | buf[2] << 8
         | buf[3] << 0;
}

int16_t get_velocity_from_response(const tmc429_trans_buf &buf) {
    return int12_to_int16(buf[2] << 8
                        | buf[3] << 0);
}

uint8_t get_interrupt_flags_from_response(const tmc429_trans_buf &buf) {
    return buf[3];
}

} // namespace parse_transaction
} // namespace TMC429
} // namespace COMMON
