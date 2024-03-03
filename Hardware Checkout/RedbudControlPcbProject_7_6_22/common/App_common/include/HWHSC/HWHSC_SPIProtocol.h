/**
 * @file HWHSC_SPIProtocol.h
 *
 * @author awong
 *
 * Main SPI protocol header for Honeywell HSC digital output pressure sensor.
 * See the Honeywell SPI comms datasheet for more information.
 *
 * Note: Expected SPI parameters:
 * SCLK between 50 and 800 kHz.
 * Polarity is LOW.
 * Data valid edge is FIRST EDGE.
 */

#ifndef APP_COMMON_INCLUDE_HWHSC_HWHSC_SPIPROTOCOL_H_
#define APP_COMMON_INCLUDE_HWHSC_HWHSC_SPIPROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <array>
#include <algorithm>

#include "SPIPeriph.h"

#include "HWHSC_TempSensor.h"

namespace COMMON {
namespace HWHSC_SPI {

// Preferring to err on the side of too much data rather than not enough, so we'll be obtaining
// all Status/Bridge data/Temperature data (4 bytes).
static constexpr size_t hwhsc_transaction_length = 4;
using hwhsc_trans_buf = std::array<uint8_t, hwhsc_transaction_length>; // Workable buffer for HWHSC.

// SPI Chip select timing from datasheet.
static constexpr uint32_t CSSetupTime_us = 3; // Rounded up from 2.5 us.
static constexpr uint32_t CSHoldTime_us = 1; // Rounded up from 0.1 us.
static constexpr uint32_t BusFreeTime_us = 2; // Noted in datasheet, but unused in the application for now.

static constexpr uint32_t StartupTimeMax_ms = 3; // Power up to data ready delay.

//! SPI configuration structure for HWHSC.
static constexpr COMMON::SPI::SpiConfig hwhsc_spi_config = {
    //! setup_time_us
    CSSetupTime_us,
    //! hold_time_us
    CSHoldTime_us,
    //! clock_polarity
    COMMON::SPI::ClockPolarity::Low,
    //! clock_phase
    COMMON::SPI::ClockPhase::OneEdge
};

enum Status : uint8_t {
    NormalOperation = 0x0,
    DeviceInCommandMode = 0x1,
    // Data that has already been fetched since the last measurement cycle, or data fetched before the first
    // measurement has been completed.
    StaleData = 0x2,
    DiagnosticCondition = 0x3
};

struct SensorData {
    Status status;
    uint16_t pressure;
    float temperature;
};

struct Transaction {
    hwhsc_trans_buf pack() {
        hwhsc_trans_buf buf = {};
        return buf;
    }

    SensorData parse(const hwhsc_trans_buf &buf) {
        SensorData data;
        data.status = static_cast<Status>((buf[0] >> 6) & 0x3);
        data.pressure = static_cast<uint16_t>(buf[0] & 0x3F) << 8
                | buf[1] << 0;
        uint16_t temperature_data_high = static_cast<uint16_t>(buf[2]) << 3;
        uint16_t temperature_data_low = ((buf[3] >> 5) & 0x7);
        data.temperature = TempSensor::calc_11(temperature_data_high
                | temperature_data_low);

        return data;
    }
};

} // namespace HWHSC_SPI
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_HWHSC_HWHSC_SPIPROTOCOL_H_ */
