/**
 * @file InterruptEntry.h
 *
 * @author awong
 */

#ifndef APP_COMMON_INTERRUPTENTRY_H_
#define APP_COMMON_INTERRUPTENTRY_H_

#include "qpcpp.hpp"

struct InterruptEntry {
#ifdef QK_HPP
    InterruptEntry() { QK_ISR_ENTRY(); }
    ~InterruptEntry() { QK_ISR_EXIT(); }
#else
    InterruptEntry() { ; }
    ~InterruptEntry() { ; }
#endif
    InterruptEntry(const InterruptEntry&) = delete;
    InterruptEntry(InterruptEntry&&) = delete;
    InterruptEntry operator=(const InterruptEntry&) = delete;
    InterruptEntry operator=(InterruptEntry&&) = delete;
};

#endif /* APP_COMMON_INTERRUPTENTRY_H_ */
