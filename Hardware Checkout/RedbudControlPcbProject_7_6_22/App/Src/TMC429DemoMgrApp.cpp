/**
 * @file TMC429DemoMgrApp.cpp
 *
 * @author awong
 *
 * Example AO that demonstrates a typical use-case of the TMC429Mgr interface.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "TMC429DemoMgr.h"

using namespace COMMON;

static TMC429DemoMgr l_TMC429DemoMgr;
QP::QActive * const AO_TMC429DemoMgr = &l_TMC429DemoMgr;
