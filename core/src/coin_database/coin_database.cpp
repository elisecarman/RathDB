/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <coin_database.h>
#include <rathcrypto.h>


CoinLocator::CoinLocator(
        uint32_t transaction_hash_, uint8_t output_index_)
        : transaction_hash(transaction_hash_),
          output_index(output_index_) {}

std::string CoinLocator::serialize(const CoinLocator& coin_locator) {
    return std::to_string(coin_locator.transaction_hash) + "-" +
           std::to_string(coin_locator.output_index);
}

std::unique_ptr<CoinLocator> CoinLocator::deserialize(const std::string& serialized_coin_locator) {
    std::string transaction_hash = serialized_coin_locator.substr(0, serialized_coin_locator.find("-"));
    std::string output_index = serialized_coin_locator.substr(serialized_coin_locator.find("-") + 1, serialized_coin_locator.size());
    return std::make_unique<CoinLocator>(std::stoul (transaction_hash,nullptr,0), std::stoul (output_index,nullptr,0));
}

std::string CoinLocator::serialize_from_construct(uint32_t transaction_hash, uint8_t output_index) {
    return std::to_string(transaction_hash) + "-" +
           std::to_string(output_index);
}

//CoinDatabase::CoinDatabase() : _database(std::make_unique<Database>()), _main_cache_capacity(10000), _main_cache_size(0), … {}


CoinDatabase :: CoinDatabase():

 _database(std :: make_unique<Database>()),
 _main_cache_capacity(1000),
 _main_cache_size(0),
// _mempool_cache(std :: make_unique< std::unordered_map<uint32_t,std::unique_ptr<Transaction>>>()),
 _mempool_capacity(1000),
 _mempool_size(0){}

bool CoinDatabase :: validate_block(const std::vector<std::unique_ptr<Transaction>>& transactions){
    //get the vector of transactions
    //for loop
    bool valid = true;
    for( int a = 0; a < transactions.size(); a = a + 1 ) {
        //std::unique_ptr<TransactionInput> trxIn = *transaction.transaction_inputs[a];
        Transaction trx = *transactions[a];
        valid += validate_transaction(trx); //check if we should pass in a pointer, and how

    }
}


bool CoinDatabase :: validate_transaction(const Transaction& transaction){
   // std::unique_ptr<std::vector> arr = &transaction.transaction_inputs
    //std::vector<std::unique_ptr<TransactionInput>> arr = std::move(transaction.transaction_inputs);
    bool valid = true;
    for( int a = 0; a < transaction.transaction_inputs.size(); a = a + 1 ) {
        //std::unique_ptr<TransactionInput> trxIn = *transaction.transaction_inputs[a];
        TransactionInput trxIn = *transaction.transaction_inputs[a];
        valid += validate_transactionInput(trxIn); //check if we should pass in a pointer, and how

    }
}

bool CoinDatabase :: validate_transactionInput (TransactionInput& trxIn) { //what does adding & at the end mean
    const std::string locator = CoinLocator::serialize(
            CoinLocator(trxIn.reference_transaction_hash, trxIn.utxo_index));

    if (_main_cache.contains(locator)){
        return _main_cache[locator]->is_spent;
    }
    else {
        std::string record = _database->get_safely(std::to_string(trxIn.reference_transaction_hash));
        std::unique_ptr<CoinRecord> coinRecord = CoinRecord::deserialize(record);
        if (std::find(coinRecord->utxo.begin(), coinRecord->utxo.end(),
                      trxIn.utxo_index) != coinRecord->utxo.end()){
            return true;
        } else {
            return false;
        }
        //retrieve coin associated with Input
        //this-> get _main_cache --> then use key

        //if found: is coin unspent? no need to check signature
        //MARK: DO WE MARK UTXO AS SPENT HERE --> "no"

        //if not found: query through database
        //coin database should be [transaction hash : Coin record] : Header is weird
        //--> if not nil: index into the correct vector
        //then remove from vector --> helper function?
    }
}

void CoinDatabase :: store_block(std::vector<std::unique_ptr<Transaction>> transactions){
    //store to main cache:
    //check if current size + transaction list size > capacity
    if (transactions.size() + _main_cache_size > _main_cache_capacity){
        flush_main_cache();
        store_transactions_to_main_cache(transactions);
        remove_transactions_from_mempool(transactions);
        //look for them in database and remove from vectors: helper function?
        //remember to remove from mempool
    } else {
        //no->
        //store to main cache: [string(coinlocator) : coin]
        //create coin locator string
        //create coin
        store_transactions_to_main_cache(transactions);
        remove_transactions_from_mempool(transactions);
        //store to coin database: [transaction.hash : CoinRecord]
        // loop through transaction list size to build Coin Record vectors
        //write Coin Record
        //remember to remove from mempool
    }
}

void CoinDatabase :: store_transactions_to_main_cache(std::vector<std::unique_ptr<Transaction>> transactions){

}

void CoinDatabase :: remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>>& transactions){

}

void CoinDatabase::flush_main_cache(){
    //yes -> //look at utxo spent: gather a list : should we have a ready list?
    //are we allowed to add fields in constructor?/
}



