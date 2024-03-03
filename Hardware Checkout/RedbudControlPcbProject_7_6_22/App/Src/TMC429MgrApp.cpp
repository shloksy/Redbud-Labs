/**
 * @file TMC429MgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for TMC429Mgr.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "TMC429Mgr.h"
#include "TMC429Protocol.h"

#include "main.h"

using namespace COMMON::TMC429;

#include "TMC429DriverChainTable.h"

static constexpr IfConfiguration config = {
    true, // Invert reference switch polarity.
    false,
    false,
    false,
    false,
    false, // Required to be false given current implementation (SPI Mode).
    false,
    StepperMotor1 // unused
};

static TMC429Mgr l_TMC429Mgr({TMC429_CS_GPIO_Port, TMC429_CS_Pin},
                             DriverChainTable,
                             NumMotorDrivers3,
                             config);
QP::QActive * const AO_TMC429Mgr = &l_TMC429Mgr;
