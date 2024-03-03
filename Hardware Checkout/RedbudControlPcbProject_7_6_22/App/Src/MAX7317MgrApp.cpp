/**
 * @file MAX7317MgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for MAX7317Mgr: IO expander for valve control PCB.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

// Must be defined before MAX7317Mgr.h include.
Q_DEFINE_THIS_MODULE("MAX7317MgrApp.h")

#include "MAX7317/MAX7317Mgr.h"

#include "main.h"

using namespace COMMON::MAX7317;

struct IOExpTraits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;
    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = HWHSC_SPI_TRANSACTION_COMPLETE_SIG;

    static constexpr enum_t SET_OUTPUT_SIG = IOEXP_SET_OUTPUT_SIG;

    // Depth of the SPI data buffers used within for SPIMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;

    static constexpr size_t NumberOfDevicesDaisyChained = NumberOfMax7317DaisyChained;

    static constexpr GPIO_TypeDef * cs_gpio_port() {
        return VALVE_EXPANDER1_CS_GPIO_Port;
    }
    static constexpr uint16_t cs_gpio_pin = VALVE_EXPANDER1_CS_Pin;
};

static MAX7317Mgr<IOExpTraits> l_IOExpMgr;
QP::QActive * const AO_IOExpMgr = &l_IOExpMgr;
