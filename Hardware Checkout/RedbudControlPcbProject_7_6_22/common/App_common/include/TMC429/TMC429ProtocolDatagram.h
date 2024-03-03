/**
 * @file TMC429ProtocolDatagram.h
 *
 * @author awong
 *
 * This module contains TMC429 protocol specific constants related with the
 * driver chain datagram configuration.
 *
 * The TMC429 must be configured with a table corresponding to the downstream
 * stepper motor driver SPI protocol (if not using the defaults).
 */

#ifndef INCLUDE_TMC429PROTOCOLDATAGRAM_H_
#define INCLUDE_TMC429PROTOCOLDATAGRAM_H_

namespace COMMON {
namespace TMC429 {

//! Absolute driver chain table length in bytes for the TMC429.
static constexpr size_t DriverChainTableLength = 64;

//! Datagram configuration codes. See section 11.4 of the TMC429 datasheet.
enum DatagramConfigCode : uint8_t {
    DG_CONFIG_DAC_A_0 = 0x00, /*!< DAC A 0 */
    DG_CONFIG_DAC_A_1 = 0x01, /*!< DAC A 1 */
    DG_CONFIG_DAC_A_2 = 0x02, /*!< DAC A 2 */
    DG_CONFIG_DAC_A_3 = 0x03, /*!< DAC A 3 */
    DG_CONFIG_DAC_A_4 = 0x04, /*!< DAC A 4 */
    DG_CONFIG_DAC_A_5 = 0x05, /*!< DAC A 5 */
    DG_CONFIG_PH_A = 0x06, /*!< DAC A phase polarity */
    DG_CONFIG_FD_A = 0x07, /*!< DAC A fast decay */
    DG_CONFIG_DAC_B_0 = 0x08, /*!< DAC B 0 */
    DG_CONFIG_DAC_B_1 = 0x09, /*!< DAC B 1 */
    DG_CONFIG_DAC_B_2 = 0x0A, /*!< DAC B 2 */
    DG_CONFIG_DAC_B_3 = 0x0B, /*!< DAC B 3 */
    DG_CONFIG_DAC_B_4 = 0x0C, /*!< DAC B 4 */
    DG_CONFIG_DAC_B_5 = 0x0D, /*!< DAC B 5 */
    DG_CONFIG_PH_B = 0x0E, /*!< DAC B phase polarity */
    DG_CONFIG_FD_B = 0x0F, /*!< DAC B fast decay */
    DG_CONFIG_zero = 0x10, /*!< zero */
    DG_CONFIG_one = 0x11, /*!< one */
    DG_CONFIG_direction = 0x12, /*!< Sets motor direction */
    DG_CONFIG_step = 0x13 /*!< Sets step bit for step/dir controlled drivers */
};

//! Helper type for use with driver chain table configuration.
using tmc429_driver_chain_table = std::array<DatagramConfigCode, DriverChainTableLength>;

//! Bit positions for datagram bits.
enum DatagramBitPositionInByte {
    SignalCodes = 0, /*!< Bit position of signal codes (DatagramConfigCode) */
    NxM = 5 /*!< Next motor bit */
};

//! Next motor bit mask.
static constexpr uint8_t NxMMask = 1u << NxM;

} // namespace TMC429
} // namespace COMMON

#endif /* INCLUDE_TMC429PROTOCOLDATAGRAM_H_ */
