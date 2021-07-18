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
    std::string transaction_hash = serialized_coin_locator.substr(
            0, serialized_coin_locator.find("-"));
    std::string output_index = serialized_coin_locator.substr(
            serialized_coin_locator.find("-") + 1, serialized_coin_locator.size());
    return std::make_unique<CoinLocator>(std::stoul (transaction_hash,nullptr,0),
                                         std::stoul (output_index,nullptr,0));
}

std::string CoinLocator::serialize_from_construct(uint32_t transaction_hash, uint8_t output_index) {
    return std::to_string(transaction_hash) + "-" +
           std::to_string(output_index);
}


CoinDatabase :: CoinDatabase():

 _database(std :: make_unique<Database>()),
 _main_cache_capacity(1000),
 _main_cache_size(0),
 _mempool_capacity(1000),
 _mempool_size(0){}

bool CoinDatabase :: validate_block(const std::vector<std::unique_ptr<Transaction>>& transactions){
    if (transactions.size() == 0){
        return false;
    }
    for( int a = 0; a < transactions.size(); a = a + 1 ) {
        const std::unique_ptr<Transaction>& trx = std::move(transactions[a]);
        if (!validate_transaction(trx)){
            return false;
        }
    }
    return true;
}


bool CoinDatabase :: validate_transaction(const std::unique_ptr<Transaction>& transaction){
    if (transaction->transaction_inputs.size() == 0){
        return false;
    }
    for( int a = 0; a < transaction->transaction_inputs.size(); a ++ ) {

        bool valid_input = validate_transactionInput(*transaction->transaction_inputs[a]);
        if (!valid_input){
            return false;
        }
    }

    return true;
}


bool CoinDatabase :: validate_transactionInput (TransactionInput trxIn) {
    const std::string locator = CoinLocator::serialize(
            CoinLocator(trxIn.reference_transaction_hash, trxIn.utxo_index));

    if (_main_cache.contains(locator)){
        bool is_spent = _main_cache[locator]->is_spent;
        if (!is_spent){
            return true;
        }
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
    }
}


std:: tuple<uint32_t, uint32_t> CoinDatabase :: return_matching_utxo(std::unique_ptr<TransactionInput> trxIn){
    const std::string locator = CoinLocator::serialize(
            CoinLocator(trxIn->reference_transaction_hash, trxIn->utxo_index));
    if (_main_cache.contains(locator)){
        auto utxo = std::make_tuple(_main_cache[locator]->transaction_output->amount,
                               _main_cache[locator]->transaction_output->public_key);
        return utxo;
    }
    else {
        std::string record = _database->get_safely(std::to_string(trxIn->reference_transaction_hash));
        std::unique_ptr<CoinRecord> coinRecord = CoinRecord::deserialize(record);

            for( int a = 0; a < coinRecord->utxo.size(); a = a + 1 ) {
                if (coinRecord->utxo[a] == trxIn->utxo_index) {
                    return std::make_tuple(coinRecord->amounts[a], coinRecord->public_keys[a]);
                }
            }
        return std::make_tuple(0,0);
    }
}



void CoinDatabase :: store_block(std::vector<std::unique_ptr<Transaction>> transactions){
    if (transactions.size() + _main_cache_size > _main_cache_capacity){
        flush_main_cache();
        remove_transactions_from_mempool(transactions);
        store_transactions_to_main_cache(std::move(transactions));

    } else {
        remove_transactions_from_mempool(transactions);
        store_transactions_to_main_cache(std :: move(transactions));
    }
}

void CoinDatabase :: store_transactions_to_main_cache(
        std::vector<std::unique_ptr<Transaction>> transactions){
    //std :: vector<CoinRecord> records;

    for( int a = 0; a < transactions.size(); a = a + 1 ) {
        std::vector<uint32_t> utxo;
        std::vector<uint32_t> amounts;
        std::vector<uint32_t> public_keys;


        for( int i = 0; i < transactions[a]->transaction_outputs.size(); i = i + 1 ) {
            utxo.insert(utxo.end(),1,i);
            amounts.insert(amounts.end(),1, transactions[a]->transaction_outputs[i]->amount);
            public_keys.insert(public_keys.end(), 1, transactions[a]->transaction_outputs[i]->public_key);
        }

        CoinRecord record = CoinRecord(
                transactions[a]->version,
                utxo,
                amounts,
                public_keys);


       std::string hash = std :: to_string(RathCrypto::hash(
               Transaction::serialize(*transactions[a])));

       _database->put_safely(hash,CoinRecord ::serialize(record));

       CoinDatabase::store_to_cache_help(std::move(transactions[a]));
    }
}

void CoinDatabase :: store_to_cache_help(std::unique_ptr<Transaction> transaction){

    _main_cache_size += transaction->transaction_outputs.size();
    uint32_t trx_hash =RathCrypto::hash(Transaction::serialize(*transaction));

    for( int a = 0; a < transaction->transaction_outputs.size(); a++ ) {

        std::unique_ptr<Coin> coin = std::make_unique<Coin>(
                std::move(transaction->transaction_outputs[a]),
                false);


        _main_cache[CoinLocator::serialize_from_construct(trx_hash,a)] = std::move(coin);
        printf("stored one transaction");
    }

}

CoinRecord CoinDatabase :: transaction_to_coin_record(Transaction trx){
    std::vector<uint32_t> utxo;
    std::vector<uint32_t> amounts;
    std::vector<uint32_t> public_keys;

    std::vector<uint32_t>::iterator it = utxo.begin();
    for (int i = 0; i < trx.transaction_outputs.size(); i = i + 1){
        utxo.insert(it,1,i);
        amounts.insert(it,1, trx.transaction_outputs[i]->amount);
        public_keys.insert(it, 1, trx.transaction_outputs[i]->public_key);
        it++;
    }

    CoinRecord record = CoinRecord(
            trx.version, utxo, amounts, public_keys);
    return record;
}

void CoinDatabase :: remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>>& transactions){
    for( int a = 0; a < transactions.size(); a = a + 1 ) {
        uint32_t trx_hash = RathCrypto::hash(Transaction::serialize(*transactions[a]));
        if (_mempool_cache.contains(trx_hash)) {
            _mempool_cache.erase(RathCrypto::hash(Transaction::serialize(*transactions[a])));
            _mempool_size -= 1;
        }
    }
}

void CoinDatabase::flush_main_cache(){
    std::unordered_map<std::string, std::unique_ptr<Coin>>::iterator it = _main_cache.begin();
    while(it != _main_cache.end())
    {
        if (it->second->is_spent){
            std::unique_ptr<CoinLocator> locator = CoinLocator::deserialize(it->first);
            CoinDatabase::remove_coin_from_database(std::move(locator));
        }
    }
    _main_cache.clear();
    _main_cache_size = 0;
}


void CoinDatabase::remove_coin_from_database(std::unique_ptr<CoinLocator> locator){
    std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(
            _database->get_safely(std::to_string(locator->transaction_hash)));

    auto it = std::find(coin_record->utxo.begin(), coin_record->utxo.end(), locator->output_index);

    if (it != coin_record->utxo.end()) {
        printf("%o", it);
        int index = it - coin_record->utxo.begin();

        coin_record->utxo.erase(coin_record->utxo.begin() + index);
        coin_record->amounts.erase(coin_record->amounts.begin() + index);
        coin_record->public_keys.erase(coin_record->public_keys.begin() + index);
        //ToDo: check that this arithmetic checks out
    }
    _database->delete_safely(std::to_string(locator->transaction_hash));
    _database->put_safely(std::to_string(locator->transaction_hash), CoinRecord::serialize(*coin_record));
}

bool CoinDatabase::store_transaction(std::unique_ptr<Transaction> transaction){
    if (_mempool_cache.size() < _mempool_capacity){
        CoinDatabase::store_transaction_in_mempool(std::move(transaction));
    }
    _mempool_size += 1;
}

void CoinDatabase::store_transaction_in_mempool(std::unique_ptr<Transaction> transaction){
    uint32_t trx = RathCrypto::hash(Transaction::serialize(*transaction));
    _mempool_cache[trx] = std::move(transaction);
    _mempool_size += 1;
}

bool CoinDatabase::validate_and_store_block(std::vector<std::unique_ptr<Transaction>> transactions){
    if (CoinDatabase::validate_block(transactions)){
        CoinDatabase::store_block(std::move(transactions));
        return true;
    }
}


bool CoinDatabase::validate_and_store_transaction(std::unique_ptr<Transaction> transaction){
    if (CoinDatabase::validate_transaction(transaction)){
        CoinDatabase::store_transaction(std::move(transaction));
    }
}

void CoinDatabase::undo_coins(
        std::vector<std::unique_ptr<UndoBlock>> undo_blocks,
        std::vector<std::unique_ptr<Block>> blocks){

    for( int a = 0; a < undo_blocks.size(); a = a + 1 ) {
        std::unique_ptr<Block> block = std::move(blocks[a]);
        std::unique_ptr<UndoBlock> undo_block = std::move(undo_blocks[a]);
        std::vector<std::unique_ptr<Transaction>> transactions = block->get_transactions();

        for( int b = 0; b < undo_block->undo_coin_records.size(); b = b + 1 ) {
            const std::unique_ptr<UndoCoinRecord> &record = undo_block->undo_coin_records[b];
            uint32_t trx_hash = undo_block->transaction_hashes[b];

            CoinDatabase::add_utxo(record, trx_hash);
            CoinDatabase::remove_utxo(std::move(transactions[b]));

        }
    }
}


void CoinDatabase::add_utxo(const std::unique_ptr<UndoCoinRecord>& undo_coin_record, uint32_t transaction_hash) {

    std::string record = "" + _database->get(std::to_string(transaction_hash));
    if (record == ""){
        CoinRecord new_record = CoinRecord(
                undo_coin_record->version,
                undo_coin_record->utxo,undo_coin_record->amounts,
                undo_coin_record->public_keys);

        _database->put_safely(std::to_string(transaction_hash), CoinRecord::serialize(new_record));
    } else {
        for( int a = 0; a < undo_coin_record->amounts.size(); a = a + 1 ) {
            std::string locator = CoinLocator::serialize_from_construct(
                    transaction_hash,
                    undo_coin_record->amounts[a]);

            if (_main_cache.contains(locator)){
                _main_cache[locator]->is_spent = false;
            } else {

                std::unique_ptr<CoinRecord> record = CoinRecord::deserialize(
                        _database->get_safely(std::to_string(transaction_hash)));
                uint32_t utxo = undo_coin_record->utxo[a];
                uint32_t amount = undo_coin_record->amounts[a];
                uint32_t pub_key = undo_coin_record->public_keys[a];

                uint32_t index = CoinDatabase::find_index(0,utxo,record);
                ///ToDo: here we assume that we do not have to check earlier than a. is this foolproof?

                record->utxo.insert(record->utxo.begin() + index, utxo);
                record->amounts.insert(record->amounts.begin() + index, amount);
                record->public_keys.insert(record->public_keys.begin() + index, pub_key);

                _database->delete_safely(std::to_string(transaction_hash));
                _database->put_safely(std::to_string(transaction_hash), CoinRecord::serialize(*record));
            }
        }
    }
}

uint32_t CoinDatabase:: find_index( uint32_t start, uint32_t utxo, const std::unique_ptr<CoinRecord>& record){
    uint32_t  index = 0;
    while(record->utxo[index] < utxo)
    {
        index += 1;
    }
    return index;
}




void CoinDatabase::remove_utxo(std::unique_ptr<Transaction> transaction){
    uint32_t trx_hash =RathCrypto::hash(Transaction::serialize(*transaction));

    for( int a = 0; a < transaction->transaction_outputs.size(); a = a + 1 ) {
        std::string locator = CoinLocator::serialize_from_construct(trx_hash, a);

        _main_cache.erase(locator);
    }
    _database->delete_safely(std::to_string(trx_hash));
    _main_cache_size -= transaction->transaction_outputs.size();
}

bool CoinDatabase::contained_in_mempool(std::unique_ptr<Transaction> trx) {
    return _mempool_cache.contains(RathCrypto::hash(Transaction::serialize(*trx)));
}

uint32_t CoinDatabase::mempool_size() {return _mempool_size;}

void CoinDatabase::quick_cache_remove(std::string locator){
    _main_cache[locator]->is_spent = true;
}

uint32_t  CoinDatabase::cache_size(){
    return _main_cache_size;
}

std::unique_ptr<CoinRecord> CoinDatabase::get_from_database(std::string hash){
std::string record = _database->get_safely(hash);

    return CoinRecord::deserialize(record);
}
