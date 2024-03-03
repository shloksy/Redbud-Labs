/**
 * @file SPIMgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for SPIMgr.  This more or less just includes the SPI bus for now.
 *
 * Required that the SPI handle won't be used before being initialized in main() (so can't touch it during construction of
 * SPIMgr).
 *
 * TODO: Extend for multiple SPIMgrs.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "spi.h" // Contains extern of spi handles.
#include "tim.h" // Contains extern of timer handles.

// Must be defined before including SPIMgr.h.
Q_DEFINE_THIS_MODULE("SPIMgrApp.cpp")
#include "SPIMgr.h"

using namespace COMMON::SPI;

struct SPI2Traits {
    static constexpr enum_t SPI_TRANSACTION_SIG = SPI2_TRANSACTION_SIG;

    static constexpr SPI_HandleTypeDef *spi_handle = &hspi2;
    static constexpr TIM_HandleTypeDef *tim_handle = &htim3;

    // Depth of the internal defer/recall QP queue within SPIMgr.
    static constexpr size_t TransactionQueueDepth = 30;
    // Depth of the SPI data buffers used within for SPIMgr.
    static constexpr size_t MaxSPITransactionLength = AppMaxSPITransactionLength;
};

static SPIMgr<SPI2Traits> l_SPI2Mgr;
QP::QActive * const AO_SPI2Mgr = &l_SPI2Mgr;

extern "C" {
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
    if (hspi == SPI2Traits::spi_handle) {
        l_SPI2Mgr.SpiTransactionComplete();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == SPI2Traits::tim_handle) {
        l_SPI2Mgr.TimerPeriodElapsed();
    }
}
}
