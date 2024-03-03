/**
 * @file memtest.cpp
 *
 * @author awong
 *
 * Module that provides memory tests.
 */

#include <stdint.h>
#include <array>
#include <cstdio>

// TODO: This printing stuff probably doesn't belong in this driver, but using it to assist with
// troubleshooting.
#include "qspy_bsp.h"

namespace memtest {

static void print_faulting_memory(const uint32_t addr, const uint8_t val, uint8_t expected_val) {
    char buf[128];
    snprintf(buf, 128, "memfail addr=0x%08lX val=0x%02X exp_val=0x%02X", addr, val, expected_val);
    COMMON::QSpy::uartDisplayString(buf);
}

void write_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t value) {
    for (auto addr = start_addr; addr < end_addr; addr++) {
        *addr = value;
    }
}

size_t read_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t expected_value) {
    size_t number_of_failures = 0;

    for (auto addr = start_addr; addr < end_addr; addr++) {
        uint8_t val = *addr;
        if (val != expected_value)  {
            print_faulting_memory(reinterpret_cast<uint32_t>(addr), val, expected_value);
            number_of_failures++;
        }
    }

    return number_of_failures;
}

size_t write_then_read_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t expected_value) {
    write_val(start_addr, end_addr, expected_value);
    return read_val(start_addr, end_addr, expected_value);
}

size_t test_inc_val(uint8_t * const start_addr, uint8_t * const end_addr) {
    size_t number_of_failures = 0;
    auto addr = start_addr;
    const size_t length = end_addr - start_addr;

    for (size_t i = 0; i < length; i++) {
        *addr = i % 256;
        addr++;
    }

    addr = start_addr;
    for (size_t i = 0; i < length; i++) {
        uint8_t val = *addr;
        uint8_t expected_value = i % 256;
        if (val != expected_value) {
            print_faulting_memory(reinterpret_cast<uint32_t>(addr), val, expected_value);
            number_of_failures++;
        }
        addr++;
    }

    return number_of_failures;
}

} // namespace memtest
