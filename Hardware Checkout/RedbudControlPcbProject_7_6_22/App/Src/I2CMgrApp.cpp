/**
 * @file I2CMgrApp.cpp
 *
 * @author awong
 *
 * Contains application specific configuration for I2CMgr.
 *
 * Required that the I2C handle won't be used before being initialized in main() (so can't touch it during construction of
 * I2CMgr).
 *
 * To port to your own application perform the following steps:
 * - Create an I2CnTraits that inherits I2CTraits and fill in all of the parameters.
 *   The set of I2CTraits should be uniquely defined for each I2CMgr instantiated. This allows
 *   for each I2CMgr to have its own uniquely defined signals.
 * - Instantiate an I2CMgr with I2CnTraits template parameter created above.
 * - Define an AO_I2CnMgr pointer that is exposed to the application for use with QP.
 * - Declare an extern to this pointer in Project.h.
 * - Inspect each I2C handle passed into each ST ISR callback and call the appropriate I2CMgr AO's
 *   routine.  I couldn't figure out a clean way to do this with the templated I2CMgr.
 */

#include "Config.h"
#include "Project.h"

#include "qpcpp.hpp"

#include "i2c.h" // Contains extern of i2c handles.

// Must be defined before I2CMgr.h include.
Q_DEFINE_THIS_MODULE("I2CMgrApp.h")

#include "I2CMgr.h"

using namespace COMMON::I2C;

struct I2CATraits {
    static constexpr enum_t WRITE_TRANSACTION_SIG = I2C4_WRITE_TRANSACTION_SIG;
    static constexpr enum_t READ_TRANSACTION_SIG = I2C4_READ_TRANSACTION_SIG;

    static constexpr I2C_HandleTypeDef *i2c_handle = &hi2c4;

    static constexpr size_t TransactionQueueDepth = 30;
    static constexpr size_t MaxI2CTransactionLength = AppMaxI2CTransactionLength;
};

static I2CMgr<I2CATraits> l_I2CAMgr;

QP::QActive * const AO_I2CAMgr = &l_I2CAMgr;

extern "C" {
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c == I2CATraits::i2c_handle) {
        l_I2CAMgr.I2CWriteTransactionComplete();
    }
    else {
        Q_ASSERT_ID(90, false);
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c == I2CATraits::i2c_handle) {
        l_I2CAMgr.I2CReadTransactionComplete();
    }
    else {
        Q_ASSERT_ID(91, false);
    }
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c == I2CATraits::i2c_handle) {
        l_I2CAMgr.I2CAbortComplete();
    }
    else {
        Q_ASSERT_ID(92, false);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c == I2CATraits::i2c_handle) {
        l_I2CAMgr.I2CError();
    }
    else {
        Q_ASSERT_ID(93, false);
    }
}
}
