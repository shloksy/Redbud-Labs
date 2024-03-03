/**
 * @file AD7172_2Protocol.cpp
 *
 * @author awong
 */

#include "AD7172_2/AD7172_2Protocol.h"

#include <array>
#include <stddef.h>

namespace COMMON {
namespace AD7172_2 {

namespace build_transaction {

size_t send_reset(ad7172_trans_buf &buf) {
    constexpr size_t send_reset_length = 8;
    buf.fill(0xFF);
    return send_reset_length;
}

size_t read_version(ad7172_trans_buf &buf) {
    constexpr size_t read_version_length = 3;
    buf[0] = CommByte::pack(WriteEnabled, Read, ID_REG);
    buf[1] = 0;
    buf[2] = 0;
    return read_version_length;
}

} // namespace build_transaction
namespace parse_transaction {

bool is_read_version_correct(const ad7172_trans_buf &buf) {
    uint16_t version = buf[1] << 8
                     | buf[2] << 0;
    // Bits 0 through 3 are don't care (X)?
    return (version & 0xFFF0) == 0x00D0;
}

} // namespace parse_transaction

} // namespace AD7172_2
} // namespace COMMON
