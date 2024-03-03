/**
 * @file TMC429Protocol.h
 *
 * @author awong
 *
 * Main SPI protocol header for TMC429 stepper motor controller.
 * See the TMC429 datasheet for more information.
 */

#ifndef INCLUDE_TMC429PROTOCOL_H_
#define INCLUDE_TMC429PROTOCOL_H_

#include "Config.h"
#include "Project.h"

#include "TMC429ProtocolDatagram.h"

#include "SPIPeriph.h"

#include <stdint.h>

#include <array>

namespace COMMON {
namespace TMC429 {

// All SPI transactions using the TMC429 are 32-bits.
static constexpr size_t tmc429_transaction_length = 4;
using tmc429_trans_buf = std::array<uint8_t, tmc429_transaction_length>; // Workable buffer for TMC429.

// Other SPI related constants:
// These times are related with the oscillator feeding the TMC429 (from the datasheet).
static constexpr uint32_t SetupTime_TMC429ClockTicks = 3.0f;
static constexpr uint32_t HoldTime_TMC429ClockTicks = 3.0f;

static constexpr COMMON::SPI::ClockPolarity clock_polarity = COMMON::SPI::ClockPolarity::High;
static constexpr COMMON::SPI::ClockPhase clock_phase = COMMON::SPI::ClockPhase::TwoEdge;

static constexpr std::array<uint8_t, 3> TYPE_VERSION_429 = {0x42, 0x91, 0x01};

//! TMC429 registers allocated to specific motors (1-3).
enum IndividualMotorRegisters {
    X_TARGET_REG = 0x0, /*!< Current target position in units of microsteps. */
    X_ACTUAL_REG = 0x1, /*!< Current position of a stepper motor in microsteps. */
    V_MIN_REG = 0x2, /*!< Absolute velocity level at which the stepper motor can be stopped abruptly. */
    V_MAX_REG = 0x3, /*!< Maximum motor velocity. */
    V_TARGET_REG = 0x4, /*!< Current target velocity of the stepper motor. */
    V_ACTUAL_REG = 0x5, /*!< Actual velocity of the stepper motor. */
    A_MAX_REG = 0x6, /*!< Maximum acceleration of the stepper motor. */
    A_ACTUAL_REG = 0x7, /*!< Actual acceleration of the stepper motor. */
    CURRENT_SCALING_VALUES_REG = 0x8, /*!< Current scaling values of the stepper motor. */
    DECELERATION_RAMP_REG = 0x9, /*!< Contains PMUL and PDIV which are used to calculate the deceleration ramp for the stepper motor. */
    CONFIG_MODE_REG = 0xA, /*!< Contains configuration mode information for the stepper motor. */
    INTERRUPT_REG = 0xB, /*!< Contains interrupt mask and flags for the stepper motor. */
    ACCEL_PARAM_AND_STEP_RATE_REG = 0xC, /*!< Scaling factors and microstep resolution config for the stepper motor. */
    DX_REF_TOLERANCE_REG = 0xD, /*!< Tolerance around the reference position. */
    X_LATCHED_REG = 0xE, /*!< Latched X position upon change of reference switch state. */
    USTEP_COUNT_429_REG = 0xF /*!< Actual microstep count of the internal sequencer. */
};

//! TMC429 global registers.
enum CommonRegisters {
    DATAGRAM_LOW_WORD_REG = 0x0, /*!< Datagram low word. Used to store datagrams sent back from stepper motor driver chain. */
    DATAGRAM_HIGH_WORD_REG = 0x1, /*!< Datagram high word. Used to store datagrams sent back from stepper motor driver chain. */
    COVER_POSITION_AND_LEN_REG = 0x2, /*!< Signals an update of datagram words. */
    COVER_DATAGRAM_REG = 0x3, /*!< Provides for the ability to send datagrams directly to the stepper motor drivers. */
    IF_CONFIGURATION_429_REG = 0x4, /*!< Configuration register for the TMC429. */
    POS_COMP_429_REG = 0x5, /*!< Defines a position which can be used to compare to the selected motor position. */
    POS_COMP_INT_429_REG = 0x6, /*!< Contains interrupt mask and flag for position compare function. */
    //! No register for 0x7
    POWER_DOWN_REG = 0x8, /*!< Write to this register will power down the TMC429. */
    TYPE_VERSION_429_REG = 0x9, /*!< Contains TMC429 version. */
    //! No register for 0xA through 0xD
    REFERENCE_SWITCHES_REG = 0xE, /*!< Current state of the reference switches. */
    STEPPER_MOTOR_GLOBAL_PARAMETER_REG = 0xF /*!< Contains configuration bits for the driver chain among other configuration. */
};

//! Stepper motor index as used by the TMC429.
enum StepperMotorIndex {
    StepperMotor1 = 0, /*!< 1 */
    StepperMotor2, /*!< 2 */
    StepperMotor3, /*!< 3 */
    NumStepperMotors /*!< Maximum number of stepper motors that is used internally. */
};

//! Used for filling in LSMD field indicating number of motor drivers connected in a daisy chain.
enum LSMD_NumberMotorDrivers : uint8_t {
    NumMotorDrivers1 = 0, /*!< Configure for 1 motor driver in the chain. */
    NumMotorDrivers2 = 1, /*!< Configure for 2 motor drivers in the chain. */
    NumMotorDrivers3 = 2 /*!< Configure for 3 motor drivers in the chain. */
};

//! Used for filling in USRS field indicating microstep resolution.
enum MicrostepResolutionSelection : uint8_t {
    Microsteps1 = 0, /*!< 1 microstep resolution */
    Microsteps2 = 1, /*!< 1/2 microstep resolution */
    Microsteps4 = 2, /*!< 1/4 microstep resolution */
    Microsteps8 = 3, /*!< 1/8 microstep resolution */
    Microsteps16 = 4, /*!< 1/16 microstep resolution */
    Microsteps32 = 5, /*!< 1/32 microstep resolution */
    Microsteps64 = 6 /*!< 1/64 microstep resolution */
};

//! Used for filling in RAMP_MODE field (Section 8.1.11.1).
enum MotionModes : uint8_t {
    RampMode = 0, /*!< Default positioning mode which uses configured acceleration. Trapezoidal velocity profile. */
    SoftMode = 1, /*!< Same as ramp mode but decrease of velocity is done with a soft, exponentially shaped profile. */
    VelocityMode = 2, /*!< Constant velocity mode which uses target velocity. */
    HoldMode = 3 /*!< Velocity mode that ignores velocity and acceleration limits so that the user has full control. */
};

//! Used for filling current scaling parameters in IS_AGTAT, ISLEAT, and IS_V0 fields.
enum IScalePercent : uint8_t {
    IScale100 = 0, /*!< 100% */
    IScale12_5 = 1, /*!< 12.5% */
    IScale25 = 2, /*!< 25% */
    IScale37_5 = 3, /*!< 37.5% */
    IScale50 = 4, /*!< 50% */
    IScale62_5 = 5, /*!< 62.5% */
    IScale75 = 6, /*!< 75% */
    IScale87_5 = 7 /*!< 87.5% */
};

class StatusBits {
private:
    enum  StatusMask {
        INTMask = (1 << 7),
        CDGWMask = (1 << 6),
        RS3Mask = (1 << 5),
        xEQt3Mask = (1 << 4),
        RS2Mask = (1 << 3),
        xEQt2Mask = (1 << 2),
        RS1Mask = (1 << 1),
        xEQt1Mask = (1 << 0)
    };

public:
    bool INT; /*!< interrupt status */
    bool CDGW; /*!< cover datagram waiting status */
    std::array<bool, NumStepperMotors> RS; /*!< reference switch status */
    std::array<bool, NumStepperMotors> xEQt; /*!< X equals target status */

    /**
     * Takes a status byte and unpacks it into the public members (INT, CDGW, etc.).
     */
    void unpack(const uint8_t status_byte) {
        INT = status_byte & StatusMask::INTMask;
        CDGW = status_byte & StatusMask::CDGWMask;

        RS[StepperMotor1] = status_byte & StatusMask::RS1Mask;
        xEQt[StepperMotor1] = status_byte & StatusMask::xEQt1Mask;
        RS[StepperMotor2] = status_byte & StatusMask::RS2Mask;
        xEQt[StepperMotor2] = status_byte & StatusMask::xEQt2Mask;
        RS[StepperMotor3] = status_byte & StatusMask::RS3Mask;
        xEQt[StepperMotor3] = status_byte & StatusMask::xEQt3Mask;
    }
};

//! Read or write flag used when forming a SPI packet.  See Section 7.3, bit 24 in the 32 bit datagram sent to TMC429.
enum ReadOrWrite {
    Write = 0, /*!< SPI packet is marked for writing. */
    Read = 1 /*!< SPI packet is marked for reading. */
};

/**
 * Expecting two use cases...  Either packing with a specific motor register in mind, or packing with a
 * common register.  Each has their own register set (referred to as IDX and JDX in the documentation).
 */
class ControlByte {
private:
    enum ControlBytePosition {
        RW = 0,
        IDX = 1,
        SMDA = 5,
        RRS = 7
    };

    // Both use cases below fall into here, which packs the bits into an 8-bit value.
    static constexpr uint8_t pack_common(const ReadOrWrite read_or_write,
                                         const uint8_t reg_addr,
                                         const uint8_t smda) {
        return ((read_or_write & 0x1) << RW) |
               ((reg_addr      & 0xF) << IDX) |
               ((smda          & 0x3) << SMDA) |
               ((0             & 0x1) << RRS); // Datasheet indicates this is always 0?
    }

public:
    //! Use case #1: Accessing a specific stepper motor register.
    static constexpr uint8_t pack(const ReadOrWrite read_or_write,
                                  const IndividualMotorRegisters indiv_motor_reg,
                                  const StepperMotorIndex motor_index);

    //! Use case #2: Accessing a common register.
    static constexpr uint8_t pack(const ReadOrWrite read_or_write,
                                  const CommonRegisters common_reg) {
        // Common register uses 0x3 for SMDA.
        return pack_common(read_or_write, static_cast<uint8_t>(common_reg), 0x3);
    }
};

//! Interface configuration helper class to pack the configuration.
struct IfConfiguration {
    bool inv_ref; //!< Inverts all reference switch polarity.
    bool sdo_int; //!< Maps internal interrupt status to nINT_SDO_C.
    bool step_half; //!< Reduces the required step pulse bandwidth.
    bool inv_stp; //!< Inverts step pulse polarity.
    bool inv_dir; //!< Inverts direction signal polarity.
    bool en_sd; //!< 0 = SPI mode. 1 = Enables step/dir mode.
    bool en_refr; //!< Enables right reference inputs REFR1, REFR2, REFR3.

    //! Moved out of order for alignment reasons.
    StepperMotorIndex pos_comp_sel; //!< Selects a motor to use for position compare function.

    //! Packs the configuration byte.
    uint8_t pack() {
        return (inv_ref << 0)
             | (sdo_int << 1)
             | (step_half << 2)
             | (inv_stp << 3)
             | (inv_dir << 4)
             | (en_sd << 5)
             | ((pos_comp_sel & 0x3) << 6);
    }
};

namespace build_transaction {

/**
 * Builds a read version transaction.
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf read_version();

/**
 * Builds a set target position transaction.  This sets the commanded X position for the
 * indexed stepper motor. The TMC429 will actively drive the stepper motor to this position.
 *
* @note The motor must be in a position mode for this command to work.
 *
 * @param motor_index   Motor index
 * @param x_target      Desired X position of the stepper motor
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_position_target(const StepperMotorIndex motor_index,
                                     const uint32_t x_target);

/**
 * Builds a set actual position transaction.  This sets the actual X position for the
 * indexed stepper motor. This allows the application to effectively reset the
 * X position reference.
 *
 * @note This can be used to reset the reference actual X position to 0 after homing is complete.
 *
 * @param motor_index   Motor index
 * @param x_target      Desired X position of the stepper motor
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_position_actual(const StepperMotorIndex motor_index,
                                     const uint32_t x_actual);

/**
 * Builds a get actual position transaction.  This gets the actual X position for the indexed stepper
 * motor.
 *
 * @param motor_index   Motor index
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf get_position_actual(const StepperMotorIndex motor_index);

/**
 * Builds a set target velocity transaction.  This sets the commanded velocity for the
 * indexed stepper motor. The TMC429 will actively drive the stepper motor at this velocity.
 *
 * @note The motor must be in velocity mode for this command to work.
 *
 * @param motor_index   Motor index
 * @param v_target      Desired X position of the stepper motor
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_velocity_target(const StepperMotorIndex motor_index,
                                     const int16_t v_target);

/**
 * Builds a get actual velocity transaction.  This gets the actual velocity for the indexed stepper
 * motor.
 *
 * @param motor_index   Motor index
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf get_velocity_actual(const StepperMotorIndex motor_index);

/**
 * Builds a set current scaling transaction.  This will configure the scaling currents for the indexed
 * stepper motor (see Section 8.1.9 in the TMC429 datasheet).
 *
 * @param motor_index                       Motor index
 * @param i_scale_when_a_is_gt_a_threshold  Current scaling when acceleration is greater than the
 *                                          acceleration threshold (IS_AGTAT in the datasheet).
 * @param i_scale_when_a_is_lt_a_threshold  Current scaling when acceleration is less than the
 *                                          acceleration threshold (IS_ALEAT in the datasheet).
 * @param i_scale_when_velocity_is_0        Current scaling when actual velocity is 0 (IS_V0 in the
 *                                          datasheet).
 * @param a_threshold                       Acceleration threshold (A_THRESHOLD in the datasheet).
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_current_scaling(const StepperMotorIndex motor_index,
                                     const IScalePercent i_scale_when_a_is_gt_a_threshold, // IS_AGTAT
                                     const IScalePercent i_scale_when_a_is_lt_a_threshold, // IS_ALEAT
                                     const IScalePercent i_scale_when_velocity_is_0, // IS_V0
                                     const uint16_t a_threshold);

/**
 * Builds a set interrupt mask transaction.
 *
 * @param motor_index   Motor index
 * @param interrupt_mask
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_interrupt_mask(const StepperMotorIndex motor_index,
                                    const uint8_t interrupt_mask);

/**
 * Builds a get interrupt flags transaction.
 *
 * @param motor_index   Motor index
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf get_interrupt_flags(const StepperMotorIndex motor_index);

/**
 * Builds a set minimum velocity transaction.  This will set the minimum velocity of the motor, which
 * is used to determine the velocity at which the motor can be controlled directly to 0.
 *
 * @param motor_index   Motor index
 * @param v_min         Velocity minimum
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_velocity_min(const StepperMotorIndex motor_index,
                                  const uint16_t v_min);

/**
 * Builds a set maximum velocity transaction.  This will set the maximum velocity of the motor.
 *
 * @param motor_index   Motor index
 * @param v_max         Velocity maximum
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_velocity_max(const StepperMotorIndex motor_index,
                                  const uint16_t v_max);

/**
 * Builds a set maximum acceleration transaction.  This will set the maximum acceleration of the motor.
 *
 * @param motor_index   Motor index
 * @param a_max         Acceleration maximum
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_accel_max(const StepperMotorIndex motor_index,
                               const uint16_t a_max);

/**
 * Builds a set deceleration ramp transaction.  This will configure the deceleration ramp with a mantissa
 * and exponent.  See datasheet for more information.
 *
 * @param motor_index   Motor index
 * @param pmul          Deceleration mantissa
 * @param pdiv          Deceleration exponent
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_deceleration_ramp(const StepperMotorIndex motor_index,
                                       const uint8_t pmul,
                                       const uint8_t pdiv);

/**
 * Builds a set configuration mode transaction.  This will configure the mode (ramp, velocity, etc.) and
 * configure the reference switch configuration.
 *
 * @param motor_index   Motor index
 * @param ramp_mode     Ramp mode
 * @param ref_conf      Reference switch configuration
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_config_mode(const StepperMotorIndex motor_index,
                                 const MotionModes ramp_mode,
                                 const uint8_t ref_conf);

/**
 * Builds a set acceleration parameter and step rate transaction.  See section 8.1.10 in the datasheet
 * for more information.
 *
 * @param motor_index   Motor index
 * @param pulse_div     Pulse divisor which scales the acceleration parameters.
 * @param ramp_div      Ramp divisor which scales the velocity parameters.
 * @param usrs          Microstep resolution
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_accel_param_and_step_rate(const StepperMotorIndex motor_index,
                                               const uint8_t pulse_div,
                                               const uint8_t ramp_div,
                                               const MicrostepResolutionSelection usrs);

/**
 * Builds a get reference switches transaction.  Obtains the reference switch state of all stepper motors.
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf get_reference_switches();

/**
 * Builds a set global parameters transaction.  This configures the SPI clock divider and number of stepper
 * motor drivers that are in the driver chain.
 *
 * @param continuous_update Whether the motor drivers are continuously updated when velocity is 0.
 * @param clk2_div          Stepper motor driver chain clock.
 * @param lsmd              Configures how many stepper motor drivers exist in the driver chain.
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_global_parameters(const bool continuous_update,
                                       const uint8_t clk2_div,
                                       const LSMD_NumberMotorDrivers lsmd);

/**
 * Builds a set interface configuration transaction.  This configures a number of global parameters based on
 * the application.
 *
 * @param config Configuration structure as set by the application.
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_if_configuration(IfConfiguration config);

/**
 * Builds a set driver chain datagram transaction.  This configures the TMC429 driver chain table as per sections
 * 11.3 and 11.4 in the datasheet.
 *
 * @param datagram_index    Datagram index
 * @param signal_code0      Signal code used for byte 0 at datagram_index.
 * @param signal_code1      Signal code used for byte 1 at datagram_index.
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_driver_chain_datagram(const uint8_t datagram_index,
                                           const uint8_t signal_code0,
                                           const uint8_t signal_code1);

/**
 * Builds a set general command.  This forms a raw 32-bit packet as per Section 7.3, TMC429 register mapping.
 *
 * @note This isn't really recommended for normal operation, but can be useful for testing or introducing
 * set commands that aren't supported yet.
 *
 * @param address   Address byte (bits 31-24)
 * @param byte0     Byte 0 (bits 23-16)
 * @param byte1     Byte 1 (bits 15-8)
 * @param byte2     Byte 2 (bits 7-0)
 *
 * @returns SPI transaction buffer
 */
tmc429_trans_buf set_general_command(const uint8_t address,
                                     const uint8_t byte0,
                                     const uint8_t byte1,
                                     const uint8_t byte2);


/**
 * Validates whether read version is correct (from read_version"()").
 *
 * @param read_buf Read buffer returned from SPI.
 *
 * @retval true  Read version matches expected.
 * @retval false Read version does not match expected.
 */
bool is_read_version_correct(const tmc429_trans_buf &read_buf);

} // namespace build_transaction

namespace parse_transaction {

/**
 * Obtains position from response packet (from get_position_actual()).
 *
 * @param buf Read buffer returned from SPI.
 *
 * @returns Actual X position
 */
uint32_t get_position_from_response(const tmc429_trans_buf &buf);

/**
 * Obtains velocity from response packet (from get_velocity_actual()).
 *
 * @param buf Read buffer returned from SPI.
 *
 * @returns Actual velocity
 */
int16_t get_velocity_from_response(const tmc429_trans_buf &buf);

/**
 * Obtains interrupt flags from response packet (from get_interrupt_flags()).
 *
 * @param buf Read buffer returned from SPI.
 *
 * @returns Raw interrupt flags byte
 */
uint8_t get_interrupt_flags_from_response(const tmc429_trans_buf &buf);
}

} // namespace TMC429
} // namespace COMMON

#endif /* INCLUDE_TMC429PROTOCOL_H_ */
