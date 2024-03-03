/**
 * @file TMC429DriverChainTable.h
 *
 * @author awong
 *
 * This configures the TMC429 motor controller driver chain table to communicate with a set
 * of 3 daisy-chained TMC249 motor drivers.
 *
 * TODO: Steps to generate the table.
 */

#ifndef INC_TMC429DRIVERCHAINTABLE_H_
#define INC_TMC429DRIVERCHAINTABLE_H_

#include "Config.h"
#include "Project.h"

#include "TMC429Protocol.h"

#include <stdint.h>

// This file should only be included in a source file that has "using namespace COMMON::TMC429;" before
// including.  This really should just be the TMC429MgrApp.cpp file.  Choosing to keep it somewhat
// logically separated for now.
constexpr tmc429_driver_chain_table DriverChainTable = {
    // TMC249 Motor Driver #1:
    // Mixed decay enable phase A.
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_A_5,
    DG_CONFIG_DAC_A_4,
    DG_CONFIG_DAC_A_3,
    DG_CONFIG_DAC_A_2,
    DG_CONFIG_PH_A,
    // Mixed decay enable phase B
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_B_5,
    DG_CONFIG_DAC_B_4,
    DG_CONFIG_DAC_B_3,
    DG_CONFIG_DAC_B_2,
    static_cast<DatagramConfigCode>(DG_CONFIG_PH_B | NxMMask), // 12

    // TMC249 Motor Driver #2:
    // Mixed decay enable phase A.
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_A_5,
    DG_CONFIG_DAC_A_4,
    DG_CONFIG_DAC_A_3,
    DG_CONFIG_DAC_A_2,
    DG_CONFIG_PH_A,
    // Mixed decay enable phase B.
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_B_5,
    DG_CONFIG_DAC_B_4,
    DG_CONFIG_DAC_B_3,
    DG_CONFIG_DAC_B_2,
    static_cast<DatagramConfigCode>(DG_CONFIG_PH_B | NxMMask), // 24

    // TMC249 Motor Driver #3:
    // Mixed decay enable phase A.
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_A_5,
    DG_CONFIG_DAC_A_4,
    DG_CONFIG_DAC_A_3,
    DG_CONFIG_DAC_A_2,
    DG_CONFIG_PH_A,
    // Mixed decay enable phase B.
#if CONFIG_ENABLE_TMC249_MIXED_DECAY
    DG_CONFIG_one,
#else
    DG_CONFIG_zero,
#endif
    DG_CONFIG_DAC_B_5,
    DG_CONFIG_DAC_B_4,
    DG_CONFIG_DAC_B_3,
    DG_CONFIG_DAC_B_2,
    static_cast<DatagramConfigCode>(DG_CONFIG_PH_B | NxMMask), // 36

    // Fill up to 64:
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, // 40
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, // 45
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, // 50
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, // 55
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, // 60
    DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one, DG_CONFIG_one // 64
};

#endif /* INC_TMC429DRIVERCHAINTABLE_H_ */
