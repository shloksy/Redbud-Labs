/**
 * @file DigitalOutputApp.h
 *
 * @author dsmoot
 * @author awong
 *
 * Application definitions for output pins that are accessible using DigitalOutputMgr.
 */

#ifndef INC_DIGITALOUTPUTAPP_H_
#define INC_DIGITALOUTPUTAPP_H_

enum DigitalOutPinIndex {
    OpticalLed1 = 0,
    OpticalLed2,
    OpticalLed3,
    RPIPowerEnable,
    Heater1,
    Heater2,
    PumpOn,
    num_digital_outputs // Must be last!
};

#endif /* INC_DIGITALOUTPUTAPP_H_ */
