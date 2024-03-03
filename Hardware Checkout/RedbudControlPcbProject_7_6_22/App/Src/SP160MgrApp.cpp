/**
 * @file SP160MgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for SP160Mgr.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

// Must be defined before SP160Mgr.h include.
Q_DEFINE_THIS_MODULE("SP160MgrApp.h")

#include "SP160/SP160Mgr.h"

#include "main.h"

using namespace COMMON::SP160;

struct SP160RegulatorTraits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;

    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = SP160_REGULATOR_TRANSACTION_COMPLETE_SIG;

    // Depth of the I2C data buffers used within for I2CMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;

    static constexpr GPIO_TypeDef * cs_gpio_port() {
        return REGULATOR_CS_GPIO_Port;
    }
    static constexpr uint16_t cs_gpio_pin = REGULATOR_CS_Pin;

    // NOTE: This is shared between regulator and pump SP160.  Both just set it high for not, so
    // no real conflict (yet).
    static constexpr GPIO_TypeDef * reset_gpio_port() {
        return SP_RESET_GPIO_Port;
    }
    static constexpr uint16_t reset_gpio_pin = SP_RESET_Pin;

    static constexpr SP160Config config = {
        PressureRange1_inH2O,
        BWLimit25_Hz,
        false,
        AZModeStandard,
        false,
        0
    };
};

struct SP160PumpTraits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;

    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = SP160_PUMP_TRANSACTION_COMPLETE_SIG;

    // Depth of the I2C data buffers used within for I2CMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;

    static constexpr GPIO_TypeDef * cs_gpio_port() {
        return PUMP_CS_GPIO_Port;
    }
    static constexpr uint16_t cs_gpio_pin = PUMP_CS_Pin;

    // NOTE: This is shared between regulator and pump SP160.  Both just set it high for not, so
    // no real conflict (yet).
    static constexpr GPIO_TypeDef * reset_gpio_port() {
        return SP_RESET_GPIO_Port;
    }
    static constexpr uint16_t reset_gpio_pin = SP_RESET_Pin;

    static constexpr SP160Config config = {
        PressureRange1_inH2O,
        BWLimit25_Hz,
        false,
        AZModeStandard,
        false,
        0
    };
};

static SP160Mgr<SP160RegulatorTraits> l_SP160RegulatorMgr;
QP::QActive * const AO_SP160RegulatorMgr = &l_SP160RegulatorMgr;

static SP160Mgr<SP160PumpTraits> l_SP160PumpMgr;
QP::QActive * const AO_SP160PumpMgr = &l_SP160PumpMgr;
