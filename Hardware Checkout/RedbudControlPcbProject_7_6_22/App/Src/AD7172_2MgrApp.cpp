/**
 * @file AD7172_2MgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for AD7172_2Mgr.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

// Must be defined before AD7172_2Mgr.h include.
Q_DEFINE_THIS_MODULE("AD7172_2MgrApp.h")

#include "AD7172_2/AD7172_2Mgr.h"

#include "main.h"

using namespace COMMON::AD7172_2;

struct AD7172ATraits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;

    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = AD7172_2_TRANSACTION_COMPLETE_SIG;

    // Depth of the I2C data buffers used within for I2CMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;

    static constexpr GPIO_TypeDef * cs_gpio_port() {
        return AD7172_CS_GPIO_Port;
    }
    static constexpr uint16_t cs_gpio_pin = AD7172_CS_Pin;
};

static AD7172_2Mgr<AD7172ATraits> l_AD7172_2Mgr;
QP::QActive * const AO_AD7172_2Mgr = &l_AD7172_2Mgr;
