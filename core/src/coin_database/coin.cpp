#include <coin.h>

Coin::Coin(
        std::unique_ptr<TransactionOutput> transaction_output_, bool is_spent_)
        : transaction_output (std::move (transaction_output_)), //dunno how to deal with this
        is_spent (is_spent_) {}


//CoinLocator::CoinLocator(
//        uint32_t transaction_hash_, uint8_t output_index_)
//        : transaction_hash(transaction_hash_),
//          output_index(output_index_) {}