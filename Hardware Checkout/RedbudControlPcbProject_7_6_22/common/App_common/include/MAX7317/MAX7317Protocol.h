/**
 * @file MAX7317Protocol.h
 *
 * @author awong
 *
 * SPI protocol header for MAX7317 IO expander.
 * See the MAX7317 datasheet for more information.
 *
 * Note: This protocol is based on a single device in the sequence.  Its possible to
 * daisy chain multiple MAX7317s together.
 */

#ifndef APP_COMMON_INCLUDE_MAX7317_MAX7317PROTOCOL_H_
#define APP_COMMON_INCLUDE_MAX7317_MAX7317PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <array>

#include "SPIPeriph.h"

namespace COMMON {
namespace MAX7317 {

// All SPI transactions to the MAX7317 are 2 bytes.
static constexpr size_t max7317_transaction_length = 2;
using max7317_trans_buf = std::array<uint8_t, max7317_transaction_length>; // Workable buffer for MAX7317.

//! SPI configuration structure for MAX7317.
static constexpr COMMON::SPI::SpiConfig max7317_spi_config = {
    //! setup_time_us
    //! Reportedly tCSS is 9.5 ns (setup):
    1,
    //! hold_time_us
    //! Reportedly tCSH is 2.5 ns (hold):
    1,
    //! clock_polarity
    COMMON::SPI::ClockPolarity::Low,
    //! clock_phase
    COMMON::SPI::ClockPhase::OneEdge
};

// No reset delay reported in the datasheet, so adding some just in case:
static constexpr uint32_t ResetDelayTime_ms = 5;

static constexpr int NumberOfPorts = 10;

enum RegisterAddress : uint8_t {
    PortOutputBaseRegisterAddress = 0x00,
    PortOutputLevel0 = 0x00,
    PortOutputLevel1 = 0x01,
    PortOutputLevel2 = 0x02,
    PortOutputLevel3 = 0x03,
    PortOutputLevel4 = 0x04,
    PortOutputLevel5 = 0x05,
    PortOutputLevel6 = 0x06,
    PortOutputLevel7 = 0x07,
    PortOutputLevel8 = 0x08,
    PortOutputLevel9 = 0x09,
};

enum ReadOrWrite : bool {
    ReadRegister = true,
    WriteRegister = false
};

class Transaction {
public:
    static max7317_trans_buf pack_enable_output(int output_port, bool enabled) {
        max7317_trans_buf buf;
        buf[0] = ((PortOutputBaseRegisterAddress + output_port) & 0x7F) | (WriteRegister << 7);
        buf[1] = enabled;
        return buf;
    }
};

} // namespace MAX7317
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_MAX7317_MAX7317PROTOCOL_H_ */
