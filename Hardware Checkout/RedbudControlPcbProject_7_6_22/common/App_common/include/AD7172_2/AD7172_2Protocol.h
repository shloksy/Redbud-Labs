/**
 * @file AD7172_2Protocol.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INCLUDE_AD7172_2_AD7172_2PROTOCOL_H_
#define APP_COMMON_INCLUDE_AD7172_2_AD7172_2PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <array>

#include "SPIPeriph.h"

namespace COMMON {
namespace AD7172_2 {

//! All SPI transactions using the AD7172 are up to 8 bytes (8 consecutive 8-bits of high is a reset).
static constexpr size_t ad7172_transaction_length = 8;
using ad7172_trans_buf = std::array<uint8_t, ad7172_transaction_length>; //! Workable buffer for AD7172.

//! SPI configuration structure for AD7172_2.
static constexpr COMMON::SPI::SpiConfig ad7172_spi_config = {
    //! setup_time_us
    //! @todo Pick real values!
    200,
    //! hold_time_us
    //! @todo Pick real values!
    200,
    //! clock_polarity
    COMMON::SPI::ClockPolarity::High,
    //! clock_phase
    COMMON::SPI::ClockPhase::TwoEdge
};

enum ADRegisters : uint8_t {
    COMMS_REG = 0x00,
    STATUS_REG = 0x00,
    ADCMODE_REG = 0x01,
    IFMODE_REG = 0x02,
    REGCHECK_REG = 0x03,
    DATA_REG = 0x04,
    GPIOCON_REG = 0x06,
    ID_REG = 0x07,
    CH0_REG = 0x10,
    CH1_REG = 0x11,
    CH2_REG = 0x12,
    CH3_REG = 0x13,
    SETUPCON0_REG = 0x20,
    SETUPCON1_REG = 0x21,
    SETUPCON2_REG = 0x22,
    SETUPCON3_REG = 0x23,
    FILTCON0_REG = 0x28,
    FILTCON1_REG = 0x29,
    FILTCON2_REG = 0x2A,
    FILTCON3_REG = 0x2B,
    OFFSET0_REG = 0x30,
    OFFSET1_REG = 0x31,
    OFFSET2_REG = 0x32,
    OFFSET3_REG = 0x33,
    GAIN0_REG = 0x38,
    GAIN1_REG = 0x39,
    GAIN2_REG = 0x3A,
    GAIN3_REG = 0x3B
};

enum WriteEnable {
    WriteEnabled = 0,
    WriteDisabled = 1
};

enum ReadOrWrite {
    Write = 0,
    Read = 1
};

// Comm byte:
// All access to the on-chip registers must start with a write to the communications register.  This write determines
// what register is accessed next and whether that operation is a write or a read.
class CommByte {
private:
    enum CommBytePosition {
        WEN = 7,
        RW = 6,
        RA = 0 // Bits 5 through 0.
    };

public:
    // Both use cases below fall into here, which packs the bits into an 8-bit value.
    static constexpr uint8_t pack(WriteEnable write_enable,
                                  ReadOrWrite read_or_write,
                                  ADRegisters reg_addr) {
        return ((write_enable  & 0x01) << WEN) |
                ((read_or_write & 0x01) << RW) |
                ((reg_addr      & 0x3F) << RA);
    }
};

namespace build_transaction {

size_t send_reset(ad7172_trans_buf &buf);
size_t read_version(ad7172_trans_buf &buf);

} // namespace build_transaction

namespace parse_transaction {

bool is_read_version_correct(const ad7172_trans_buf &buf);

} // namespace parse_transaction

} // namespace AD7172_2
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_AD7172_2_AD7172_2PROTOCOL_H_ */
