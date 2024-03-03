/**
 * @file TTPVentusUartProtocol.h
 *
 * @author awong
 *
 * Main UART protocol header for TTP Ventus disc pump.
 * See the TTP Ventus Technical Note TN0003: Serial Communications Guide for more
 * information.
 */

#ifndef APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSUARTPROTOCOL_H_
#define APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSUARTPROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

namespace COMMON {
namespace TTPV {

// Should be set to the maximum, or greater than the maximum string we'll need to send or
// receive from the TTP Ventus.
static constexpr size_t maximum_uart_length = 64;
using ttpv_scratch_buf = std::array<uint8_t, maximum_uart_length>; // Workable buffer for TTPVentus.

enum CommandId : uint8_t {
    PumpEnabled = 0,
    PowerLimit = 1,
    StreamMode = 2,
    DriveVoltage = 3,
    DriveCurrent = 4,
    DrivePower = 5,
    DriveFrequency = 6,
    // TODO: Add missing command IDs.  Only adding ones we might use for now.

    MajorFirmwareVersion = 36,
    FirmwareDeviceType = 37,
    MinorFirmwareVersion = 38
};

// TODO: For now, just use hard-coded strings for UART transactions.
static constexpr char read_major_version_string[] = "#R36\n";
static constexpr char read_firmware_device_type_string[] = "#R37\n";
static constexpr char read_minor_version_string[] = "#R38\n";

static constexpr char write_enable_stream_mode[] = "#R2,1\n";

} // namespace TTPV
} // namespace COMMON



#endif /* APP_COMMON_INCLUDE_TTPVENTUS_TTPVENTUSUARTPROTOCOL_H_ */
