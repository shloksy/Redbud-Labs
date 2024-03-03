/**
 * @file MAX7315MgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for MAX7315Mgr: IO expander for valve control PCB.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

// Must be defined before MAX7315Mgr.h include.
Q_DEFINE_THIS_MODULE("MAX7315MgrApp.h")

#include "MAX7315/MAX7315Mgr.h"

#include "main.h"

using namespace COMMON::MAX7315;

struct IOExpTraits {
    static constexpr enum_t I2C_WRITE_SIG = I2C4_WRITE_TRANSACTION_SIG;
    static constexpr enum_t I2C_READ_SIG = I2C4_READ_TRANSACTION_SIG;
    static constexpr enum_t I2C_COMPLETE_SIG = IOEXP2_I2C_TRANSACTION_COMPLETE_SIG;

    static constexpr enum_t SET_OUTPUT_SIG = IOEXP2_SET_OUTPUT_SIG;

    // Depth of the I2C data buffers used within for I2CMgr.
    static constexpr size_t MaxI2CTransactionLength = AppMaxI2CTransactionLength;

    static constexpr uint8_t I2CAddress = 0x40; //! All address pins pulled to ground.
    static constexpr uint8_t IsInputPinConfiguration = 0x00; //! Bitmask of all 8 pins and whether or not they're input.
};

static MAX7315Mgr<IOExpTraits> l_IOExpMgr2;
QP::QActive * const AO_IOExpMgr2 = &l_IOExpMgr2;
