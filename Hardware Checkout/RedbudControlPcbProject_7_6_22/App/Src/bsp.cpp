/**
 * @file bsp.cpp
 *
 * @author awong
 */

#include "Config.h"

#include "qpcpp.hpp"
#include "bsp.h"
#include "main.h"
#include "qspy_bsp.h"
#include "debug_uart_drv.h"

#include "sdram_test.h"

#include "memtest.h"

#include "sys_app.h"

#include <cstring>

Q_DEFINE_THIS_MODULE("bsp.cpp")

namespace COMMON {

#ifdef Q_SPY
BSP l_BSP {};
#endif

static char projMsg[]  = "CoreH743I Sparx Controller";
static char starMsg[]  = "**************************";

uint32_t BSP::HAL_GetTick(void) {
    return uwTick;
}

void BSP::init(void) {
#ifdef Q_SPY
    COMMON::DebugUart::StartInterruptReceiveOfSingleByteOnUart();
#endif

    QS_OBJ_DICTIONARY(&COMMON::l_BSP);

    COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(starMsg);
    COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(projMsg);
    COMMON::QSpy::uartDisplayStringWithNewLineExceptOnQspy(starMsg);

    sys_app::reset_reason_print();

#if CONFIG_ENABLE_BENCHMARKING_SDRAM_ON_BOOTUP
    sdram_test::benchmark_sdram();
#endif

#if CONFIG_ENABLE_SDRAM_TEST_ON_BOOTUP
    // Memory test on the SDRAM:
    uint8_t * sdram_start_addr = reinterpret_cast<uint8_t *>(0xD0000000);
    uint8_t * sdram_end_addr = reinterpret_cast<uint8_t *>(0xD0800000);
    for (size_t i = 0; i < 50; i++) {
        //		Q_ASSERT_ID(5, memtest::test_inc_val(sdram_start_addr, sdram_end_addr) == 0);
        // Test refresh:
        COMMON::QSpy::uartDisplayString("SDRAM Refresh Test:");
        COMMON::QSpy::uartDisplayString("SDRAM writing 0xFF...");
        memtest::write_val(sdram_start_addr, sdram_end_addr, 0xFF);
        COMMON::QSpy::uartDisplayString("Delay 10 seconds...");
        HAL_Delay(10000);
        COMMON::QSpy::uartDisplayString("SDRAM reading expecting 0xFF...");
        Q_ASSERT_ID(5, memtest::read_val(sdram_start_addr, sdram_end_addr, 0xFF) == 0);

        COMMON::QSpy::uartDisplayString("SDRAM writing then reading 0xA5...");
        Q_ASSERT_ID(10, memtest::write_then_read_val(sdram_start_addr, sdram_end_addr, 0xA5) == 0);
        COMMON::QSpy::uartDisplayString("SDRAM writing then reading 0x00...");
        Q_ASSERT_ID(15, memtest::write_then_read_val(sdram_start_addr, sdram_end_addr, 0x00) == 0);
        COMMON::QSpy::uartDisplayString("SDRAM writing then reading 0xFF...");
        Q_ASSERT_ID(20, memtest::write_then_read_val(sdram_start_addr, sdram_end_addr, 0xFF) == 0);
        COMMON::QSpy::uartDisplayString("SDRAM writing then reading inc...");
        Q_ASSERT_ID(25, memtest::test_inc_val(sdram_start_addr, sdram_end_addr) == 0);
    }
#endif
}

void BSP::onIdle() {
}

void BSP::BSP_ledOn()
{
    HAL_GPIO_WritePin(BSP_LED_PORT, BSP_LED_PIN, GPIO_PIN_SET);
}

void BSP::BSP_ledOff()
{
    HAL_GPIO_WritePin(BSP_LED_PORT, BSP_LED_PIN, GPIO_PIN_RESET);
}

} //namespace COMMON
