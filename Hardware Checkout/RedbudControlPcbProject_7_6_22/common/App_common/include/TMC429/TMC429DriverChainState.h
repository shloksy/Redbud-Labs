/**
 * @file TMC429DriverChainState.h
 *
 * @author awong
 */

#ifndef INCLUDE_TMC429_TMC429DRIVERCHAINSTATE_H_
#define INCLUDE_TMC429_TMC429DRIVERCHAINSTATE_H_

#include "Config.h"
#include "Project.h"
#include "qpcpp.hpp"

#include "TMC429Protocol.h"

namespace COMMON {
namespace TMC429 {

class DriverChainTableState {
private:
    static constexpr size_t NumberOfTransactions = DriverChainTableLength / 2;

    // Each transaction uses 2 entries from the table.
    size_t transaction_count; // Number of transactions built.
    size_t completed_transactions; // Number of transactions completed.
    size_t table_index; // Index into the actual driver chain table.
    const tmc429_driver_chain_table &table;

public:
    DriverChainTableState(const tmc429_driver_chain_table &driver_chain_table) :
        transaction_count(0),
        completed_transactions(0),
        table_index(0),
        table(driver_chain_table)
    {
        ;
    }

    void reset() {
        transaction_count = 0;
        completed_transactions = 0;
        table_index = 0;
    }

    tmc429_trans_buf build_next_datagram() {
        const auto signal_code0 = get_next_table_entry();
        const auto signal_code1 = get_next_table_entry();

        return build_transaction::set_driver_chain_datagram(
                transaction_count++,
                signal_code0,
                signal_code1);
    }

    void mark_transaction_complete() {
        completed_transactions++;
    }

    bool is_driver_chain_complete() {
        return (transaction_count >= NumberOfTransactions)
            && (completed_transactions >= NumberOfTransactions);
    }

private:
    uint8_t get_next_table_entry() {
        return table[table_index++];
    }
};

} // namespace TMC429
} // namespace COMMON

#endif /* INCLUDE_TMC429_TMC429DRIVERCHAINSTATE_H_ */
