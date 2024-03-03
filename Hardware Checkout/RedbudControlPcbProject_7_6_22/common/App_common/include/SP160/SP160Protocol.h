/**
 * @file SP160Protocol.h
 *
 * @author awong
 *
 * Main SPI protocol header for SP160 Superior differential low pressure sensors.
 * See the SP160 datasheet for more information.
 *
 * TODO: It looks like this protocol actually will work for the SP110, SP210, and SP160 series except for
 * the PressureRangeSelectOptions field.
 */

#ifndef APP_COMMON_INCLUDE_SP160_SP160PROTOCOL_H_
#define APP_COMMON_INCLUDE_SP160_SP160PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <array>
#include <algorithm>

#include "SPIPeriph.h"

namespace COMMON {
namespace SP160 {

// TODO: Should we provide only pressure as well? (ignoring temperature sensor).
// All SPI transactions to the SP160 are 4 bytes if obtaining pressure and temperature.
static constexpr size_t sp160_transaction_length = 4;
using sp160_trans_buf = std::array<uint8_t, sp160_transaction_length>; // Workable buffer for SP160.
static constexpr size_t sp160_init_transaction_length = 20;
// Initial buffer that is used to collect model, serial number, build number.
using sp160_init_trans_buf = std::array<uint8_t, sp160_init_transaction_length>;

//! SPI configuration structure for SP160.
static constexpr COMMON::SPI::SpiConfig sp160_spi_config = {
    //! setup_time_us
    100,
    //! hold_time_us
    20,
    //! clock_polarity
    COMMON::SPI::ClockPolarity::Low,
    //! clock_phase
    COMMON::SPI::ClockPhase::TwoEdge
};

// First Response Settling Time from datasheet (greater of the two reset-ish timings).
static constexpr uint32_t ResetDelayTime_ms = 60;

enum PressureRangeSelectOptions : uint8_t {
    PressureRange1_inH2O = 0x0,
    PressureRange2_inH2O = 0x1,
    PressureRange5_inH2O = 0x2,
    PressureRange10_inH2O = 0x3
};

enum BWLimitSelectOptions : uint8_t {
    BWLimit25_Hz = 0x0,
    BWLimit35_Hz = 0x1,
    BWLimit50_Hz = 0x2,
    BWLimit65_Hz = 0x3,
    BWLimit100_Hz = 0x4,
    BWLimit130_Hz = 0x5,
    BWLimit180_Hz = 0x6,
    BWLimit250_Hz = 0x7
};

enum AZModeOptions : bool {
    AZModeStandard = 0x0,
    AZModeZTrack = 0x1
};

struct SP160Config {
    PressureRangeSelectOptions pressure_range;
    BWLimitSelectOptions bw_limit_select;
    bool auto_zero_enable;
    AZModeOptions auto_zero_mode;
    bool zero_noise_suppression_enable;
    uint8_t data_rate_divisor;
};

static constexpr size_t model_length = 6;
static constexpr size_t serial_number_length = 4;
static constexpr size_t build_number_length = 6;

struct ExtendedData {
    std::array<char, model_length> model;
    std::array<uint8_t, serial_number_length> serial_number;
    std::array<char, build_number_length> build_number;
};

static constexpr std::array<char, model_length> sp160_model =
{'S', 'P', '1', '6', '0', '\0'};

class Transaction {
private:
    SP160Config cached_config;

public:
    // Designed in this way because these values don't typically change
    // very often (if ever) at run-time.  Application can set it once and
    // continuously call pack() when building a SPI transaction.
    Transaction(const SP160Config &config) {
        init(config);
    }

    void init(const SP160Config &config) {
        cached_config = config;
    }

    sp160_trans_buf pack_sensor_transaction() {
        sp160_trans_buf buf = {};
        buf[0] = (cached_config.pressure_range & 0x3) << 0
                | (cached_config.bw_limit_select & 0x7) << 2
                | (cached_config.auto_zero_enable & 0x1) << 5
                | (cached_config.auto_zero_mode & 0x1) << 6
                | (cached_config.zero_noise_suppression_enable & 0x1) << 7;
        buf[1] = cached_config.data_rate_divisor;
        return buf;
    }

    sp160_init_trans_buf pack_init_transaction() {
        sp160_init_trans_buf buf = {};
        buf[0] = (cached_config.pressure_range & 0x3) << 0
                | (cached_config.bw_limit_select & 0x7) << 2
                | (cached_config.auto_zero_enable & 0x1) << 5
                | (cached_config.auto_zero_mode & 0x1) << 6
                | (cached_config.zero_noise_suppression_enable & 0x1) << 7;
        buf[1] = cached_config.data_rate_divisor;
        return buf;
    }

    ExtendedData parse_init_transaction(const sp160_init_trans_buf &buf) {
        ExtendedData data;
        std::copy_n(buf.begin() + 4, data.model.size(), data.model.begin());
        std::copy_n(buf.begin() + 10, data.serial_number.size(), data.serial_number.begin());
        std::copy_n(buf.begin() + 14, data.build_number.size(), data.build_number.begin());
        return data;
    }
};

} // namespace SP160
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_SP160_SP160PROTOCOL_H_ */
