/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_COIN_DATABASE_H
#define RATHDB_STENCIL_COIN_DATABASE_H


#include <db.h>
#include <coin.h>
#include <coin_record.h>
#include <undo_block.h>
#include "../primitives/block.h"

/// Locator/Identifier for UTXO.
class CoinLocator {
public:
    const uint32_t transaction_hash;
    const uint8_t output_index;

    CoinLocator(uint32_t transaction_hash, uint8_t output_index);

    static std::string serialize_from_construct(uint32_t transaction_hash, uint8_t output_index);
    static std::string serialize(const CoinLocator& coin_locator);
    static std::unique_ptr<CoinLocator> deserialize(const std::string& serialized_coin_locator);
};

/// Database that manages transactions/UTXO (storing, validating, querying).
class CoinDatabase {
private:

    /// db (hashmap storage) of block hash to block record.
    std::unique_ptr<Database> _database;
    /// Main cache variables
    std::unordered_map<std::string, std::unique_ptr<Coin>> _main_cache;
    /// maximum amount of entries in the cache
    const uint16_t _main_cache_capacity;
    /// current number of entries in the cache
    uint16_t _main_cache_size;
    /// maps transaciton hash to transaction
    std::unordered_map<uint32_t, std::unique_ptr<Transaction>> _mempool_cache;
    /// maximum amount of entries in mempool
    const uint16_t _mempool_capacity;
    /// current amount of entires in mempool
    uint16_t _mempool_size;



public:
    CoinDatabase();

    ///validation function
    bool validate_block(const std::vector<std::unique_ptr<Transaction>>& transactions);

    bool validate_transaction(const std::unique_ptr<Transaction>& transaction);
    ///a helper to validate_transaction. called on every transaction input within a transaction
    //tested
    bool validate_transactionInput(TransactionInput trxIn);

    ///storing and validating
    void store_block(std::vector<std::unique_ptr<Transaction>> transactions);
    //tested
    void store_transactions_to_main_cache(std::vector<std::unique_ptr<Transaction>> transactions);
    ///a helper to store_transactions_to_main_cache which is applied to each
    ///transaction within the store_transactions_to_main_cache
    //tested
    void store_to_cache_help(std::unique_ptr<Transaction> transaction);
    //tested
    bool store_transaction(std::unique_ptr<Transaction> transaction);
    bool validate_and_store_block(std::vector<std::unique_ptr<Transaction>> transactions);
    bool validate_and_store_transaction(std::unique_ptr<Transaction> transaction);

    //tested
    void remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>>& transactions);

    ///obsolete
    void mark_as_spent(std::unique_ptr<Transaction> transaction);
    ///obsolete
    static CoinRecord transaction_to_coin_record(Transaction trx);

    //tested
    void store_transaction_in_mempool(std::unique_ptr<Transaction> transaction);
    //tested
    void remove_coin_from_database(std::unique_ptr<CoinLocator> locator);

    //tested
    void flush_main_cache();

    std::vector<std::pair<uint32_t, uint8_t>> get_all_utxo(uint32_t public_key);
    ///undo coins functions

    ///undo_coins takes two corresponding lists of blocks and undo blocks and updates its main cache and database.
    void undo_coins(std::vector<std::unique_ptr<UndoBlock>> undo_blocks, std::vector<std::unique_ptr<Block>> blocks);
    ///add_utxo takes in a transaction hash and a corresponding Undo Coin Record. It adds the utxo to both the main
    /// chain and the database. For the database, it either creates a Coin Record or insert utxo within existing Coin
    /// Records.
    //tested
    void add_utxo(const std::unique_ptr<UndoCoinRecord>& undo_coin_record, uint32_t transaction_hash);
    ///takes in a transaction from the old active chain. queries through its utxo to delete utxo from the main cache,
    ///and uses its transaction hash to remove CoinRecords from the database.
    //tested
    void remove_utxo(std::unique_ptr<Transaction> transaction);
    ///takes in an a hypotehtical index where the utxo may be placed, the utxo_index of the utxo, and the CoinRecord
    ///we are querying. solves this recursively.
    /// finds the index at which Transaction output elements should be inserted
    //tested
    uint32_t find_index(uint32_t index, uint32_t utxo, const std::unique_ptr<CoinRecord>& record);
    //tested
    std::tuple<uint32_t, uint32_t> return_matching_utxo(std::unique_ptr<TransactionInput>);

    ///testing helper functions

    ///returns whether a transaction is within the mempool
    bool contained_in_mempool(std::unique_ptr<Transaction>);
    ///returns the mempool size
    uint32_t mempool_size();
    ///removes a transaction from cache
    void quick_cache_remove(std::string locator);
    ///outputs the size of main cache
    uint32_t cache_size();
    ///retreives a CoinRecord from main cache
    std::unique_ptr<CoinRecord> get_from_database(std::string hash);


};




#endif //RATHDB_STENCIL_COIN_DATABASE_H
