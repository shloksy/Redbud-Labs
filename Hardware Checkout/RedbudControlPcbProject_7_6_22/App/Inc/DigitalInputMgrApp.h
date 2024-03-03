/**
 * @file DigitalInputApp.h
 *
 * @author dsmoot
 * @author awong
 */

#ifndef INC_DIGITALINPUTAPP_H_
#define INC_DIGITALINPUTAPP_H_

enum DigitalInPinIndex {
    PushButtonSwitch,
    PumpFeedback,
    RockerSwitch1,
    FanSpeedFeedback,
    MotionHallFeedback,
    MotionFeedbackA,
    MotionFeedbackB,
    MotionFeedbackC,
    RPIPwrNFlag,
    OpticalFeedback1,
    OpticalFeedback2,
    OpticalFeedback3,
    OpticalFeedback4,
	num_polled_inputs // Must be last!
};

#endif /* INC_DIGITALINPUTAPP_H_ */
