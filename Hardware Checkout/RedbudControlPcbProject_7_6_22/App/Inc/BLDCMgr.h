/**
 * @file BLDCMgr.h
 * @date 2022/04/07
 * @author rsutton
 */

#ifndef INC_BLDCMGR_H_
#define INC_BLDCMGR_H_

#include <cstdint>
#include "qpcpp.hpp"
#include "bsp.h"

namespace BLDC {

using volts = float;
using percentage = std::uint8_t;
using pulse_count = std::uint16_t;

class BLDCMgr : public QP::QActive {
private:
    static constexpr uint32_t BLDCStatusPeriodMs = 50;
    static constexpr uint32_t BLDCStatusPeriodTicks = BLDCStatusPeriodMs / COMMON::BSP::MsPerBspTick;

private:
    QP::QTimeEvt BLDCStatusTimer;
    bool is_faulted;

public:
    BLDCMgr();
    static BLDCMgr inst;

private:
    void handleSetBLDCSpeedPercentage(const SetBLDCSpeedPercentageEvent * e);
    void setVoltageOutput(percentage setpoint);
    static pulse_count percentageToPulseCount(percentage input);
    void writePulseCountToTimer(pulse_count setpoint);

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(running);
};

}

#endif /* INC_BLDCMGR_H_ */
