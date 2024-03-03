/**
 * @file TMC429Query.h
 *
 * @author awong
 *
 * Helper class that deals with state information related with status transactions.  Used
 * directly by TMC429Mgr.
 */

#ifndef APP_COMMON_INCLUDE_TMC429_TMC429QUERY_H_
#define APP_COMMON_INCLUDE_TMC429_TMC429QUERY_H_

#include "TMC429Protocol.h"

#include <stddef.h>
#include <array>

namespace COMMON {
namespace TMC429 {

/**
 * Helper struct that manages querying state (for TMC429Mgr).
 */
struct QueryState {
    StepperMotorIndex motor_index; //! Motor index of current query.

    size_t queries_sent; //! Number of queries sent.
    size_t queries_received; //! Number of queries received.

    uint32_t position; //! Position of the motor read back from query.
    int16_t velocity; //! Velocity of the motor read back from query.
    uint8_t int_flags; //! Interrupt flags of the motor read back from query.

    //! Constructor which sets the initial state of QueryState.
    QueryState() {
        reset();
    }

    //! Resets internal state.
    void reset() {
        motor_index = StepperMotor1;
        queries_sent = 0;
        queries_received = 0;
        position = 0;
        velocity = 0;
        int_flags = 0;
    }

    //! Configures which motor will be queried.
    void set_motor_index(StepperMotorIndex index) {
        motor_index = index;
    }

    //! Returns whether or not queries have completed.
    bool queries_complete() const {
        return queries_received >= queries_sent;
    }

    //! Collects the query response and increments an internal counter.
    // @todo This is a bit gross, but doing it this way for now.
    // @todo Seems a bit weird to collect the response here, but not build the packet here.
    void collect_response(const tmc429_trans_buf &buf) {
        switch(queries_received) {
            case 0:
                position = parse_transaction::get_position_from_response(buf);
                break;
            case 1:
                velocity = parse_transaction::get_velocity_from_response(buf);
                break;
            case 2:
                int_flags = parse_transaction::get_interrupt_flags_from_response(buf);
                break;
            default:
                // @todo ASSERT?  Should be impossible to get here.
                break;
        }

        queries_received++;
    }
};

} // namespace TMC429
} // namespace COMMON

#endif /* APP_COMMON_INCLUDE_TMC429_TMC429QUERY_H_ */
