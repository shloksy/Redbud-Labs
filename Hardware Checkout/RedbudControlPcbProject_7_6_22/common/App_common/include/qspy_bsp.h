/**
 * @file qspy_bsp.h
 *
 * @author dsmoot
 * @author awong
 */

#ifndef INCLUDE_QSPY_BSP_H_
#define INCLUDE_QSPY_BSP_H_

namespace COMMON {
namespace QSpy {

// Idle processing--call with interrupts disabled.
void onIdle();

bool uartDisplayString(const char* string);
bool uartDisplayText(const char* text, uint32_t length);
bool uartDisplayStringWithNewLineExceptOnQspy(const char* string);

void uartFlush();

}
}


#endif /* INCLUDE_QSPY_BSP_H_ */
