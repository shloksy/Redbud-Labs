/**
 * @file HWHSC_SPIMgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for HWHSC_SPIMgr: Honeywell HSC pressure sensor.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

// Must be defined before SP160Mgr.h include.
Q_DEFINE_THIS_MODULE("HWHSC_SPIMgrApp.h")

#include "HWHSC/HWHSC_SPIMgr.h"

#include "main.h"

using namespace COMMON::HWHSC_SPI;

struct HWHSCTraits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;
    static constexpr enum_t SPI_TRANSACTION_COMPLETE_SIG = HWHSC_SPI_TRANSACTION_COMPLETE_SIG;

    static constexpr enum_t SENSOR_DATA_SIG = HWHSC_SENSOR_DATA_SIG;

    // Depth of the SPI data buffers used within for SPIMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;

    // Period at which the sensor obtains data.
    static constexpr uint32_t QuerySensorPeriodMs = 500;

    static constexpr GPIO_TypeDef * cs_gpio_port() {
        return HSC_CS_GPIO_Port;
    }
    static constexpr uint16_t cs_gpio_pin = HSC_CS_Pin;
};

static HWHSC_SPIMgr<HWHSCTraits> l_HWHSC_SPIMgr;
QP::QActive * const AO_HWHSC_SPIMgr = &l_HWHSC_SPIMgr;
