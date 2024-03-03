/**
 * @file memtest.h
 *
 * @author awong
 */

#ifndef INCLUDE_SPARXLIB_DRIVERS_MEMTEST_H_
#define INCLUDE_SPARXLIB_DRIVERS_MEMTEST_H_

#include <stdint.h>
#include <array>

namespace memtest {

size_t write_then_read_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t val);
void write_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t value);
size_t read_val(uint8_t * const start_addr, uint8_t * const end_addr, const uint8_t expected_value);

size_t test_inc_val(uint8_t * const start_addr, uint8_t * const end_addr);

} // namespace memtest

#endif /* INCLUDE_SPARXLIB_DRIVERS_MEMTEST_H_ */
