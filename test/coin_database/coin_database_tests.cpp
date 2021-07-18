/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <gtest/gtest.h>
#include <coin_record.h>
#include <coin_database.h>
#include <rathcrypto.h>

TEST(CoinLocator, Serialize) {
    CoinLocator coin_locator = CoinLocator(40, 33);
    std::string serialized_coin_locator = CoinLocator::serialize(coin_locator);
    std::unique_ptr<CoinLocator> deserialized_coin_locator = CoinLocator::deserialize(serialized_coin_locator);
    EXPECT_EQ(coin_locator.output_index, deserialized_coin_locator->output_index);
    EXPECT_EQ(deserialized_coin_locator->output_index, 33);

    std::string serialized_coin_locator2 = CoinLocator::serialize_from_construct(22, 19);
    std::unique_ptr<CoinLocator> deserialized_coin_locator2 = CoinLocator::deserialize(serialized_coin_locator2);
    EXPECT_EQ(deserialized_coin_locator2->output_index, 19);
}

TEST(CoinRecord, Serialize) {
    std::vector<uint32_t> utxo = {99};
    std::vector<uint32_t> amounts = {12};
    std::vector<uint32_t> public_keys = {14};
    CoinRecord coin_record = CoinRecord(99, std::move(utxo), std::move(amounts),
                                        std::move(public_keys));
    std::string serialized_coin_record = CoinRecord::serialize(coin_record);
    std::unique_ptr<CoinRecord> deserialized_coin_record = CoinRecord::deserialize(serialized_coin_record);
    EXPECT_EQ(coin_record.version, deserialized_coin_record->version);
    EXPECT_EQ(deserialized_coin_record->version, 99);
}

TEST(CoinDatabase, store_transaction_in_mempool){
    std :: vector<std::unique_ptr<TransactionInput>> inputs;
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    CoinDatabase coin_database = CoinDatabase();

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std::move(inputs),
            std::move(outputs), 0, 0);

    EXPECT_TRUE(!coin_database.contained_in_mempool(std::move(transaction1)));

    std :: vector<std::unique_ptr<TransactionInput>> inputs2;
    std :: vector<std::unique_ptr<TransactionOutput>> outputs2;
    std::unique_ptr<Transaction> transaction2 = std::make_unique<Transaction>(
            std::move(inputs2),
            std::move(outputs2), 0, 0);

    coin_database.store_transaction_in_mempool(std::move(transaction2));

    std :: vector<std::unique_ptr<TransactionInput>> inputs3;
    std :: vector<std::unique_ptr<TransactionOutput>> outputs3;

    std::unique_ptr<Transaction> transaction3 = std::make_unique<Transaction>(
            std::move(inputs3),
            std::move(outputs3), 0, 0);
    EXPECT_TRUE(coin_database.contained_in_mempool(std::move(transaction3)));
}


TEST(CoinDatabase, remove_transaction_from_mempool){

    CoinDatabase coin_database = CoinDatabase();
    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 0, 0);

    std::unique_ptr<Transaction> transaction2 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 1, 1);

    coin_database.store_transaction_in_mempool(std::move(transaction1));

    std::unique_ptr<Transaction> transaction1_1 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 0, 0);

    EXPECT_EQ(coin_database.contained_in_mempool(std::move(transaction1_1)), true);

    uint32_t curr_size = coin_database.mempool_size();
    coin_database.remove_transactions_from_mempool(std::vector<std::unique_ptr<Transaction>>{});
    EXPECT_EQ(curr_size, coin_database.mempool_size());

    std::unique_ptr<Transaction> transaction1_2 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 0, 0);

    std::vector<std::unique_ptr<Transaction>> vector2;
    vector2.push_back(std::move(transaction1_2));
    vector2.push_back(std::move(transaction2));

    coin_database.remove_transactions_from_mempool(vector2); EXPECT_EQ(curr_size - 1,
                                                                       coin_database.mempool_size());
}


TEST(CoinDatabase, store_transaction){
    //transaction1
    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 0, 0);
    //transaction2
    std::unique_ptr<Transaction> transaction2 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 1, 1);
    //transaction3
    std::unique_ptr<Transaction> transaction3 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 2, 2);


    std::unique_ptr<Transaction> transaction4 = std::make_unique<Transaction>(
            std::vector<std::unique_ptr<TransactionInput>>{},
            std::vector<std::unique_ptr<TransactionOutput>>{}, 2, 2);


}



TEST(CoinDatabase, validate_transactionInput){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    Transaction tran1 = Transaction(std::move(inputs),
                                           std::move(outputs),
                                           1,
                                           1);



    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));

    uint32_t trx_hash = RathCrypto::hash(Transaction::serialize(tran1));

    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));

    TransactionInput input = TransactionInput(trx_hash,0,2);

    TransactionInput input2 = TransactionInput(trx_hash,1,5);

    TransactionInput input_false = TransactionInput(trx_hash,8,5);

    TransactionInput input_false2 = TransactionInput(0,0,5);

    //test when coin is in main cache
    EXPECT_TRUE(coin_database.validate_transactionInput(input2));

    //test when coin is in database but not cache
    EXPECT_TRUE(coin_database.validate_transactionInput(input));

    //test when we cannot find the coin- wrong utxo
    EXPECT_TRUE(!coin_database.validate_transactionInput(input_false));

    //test when we cannot find the coin- wrong transaction hash
    EXPECT_TRUE(!coin_database.validate_transactionInput(input_false2));
}


TEST(CoinDatabase, store_to_cache_help){
    CoinDatabase coin_database = CoinDatabase();

    std :: vector<std::unique_ptr<TransactionInput>> inputs;
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs),
            std :: move(outputs),
            1,
            1);

    EXPECT_EQ(coin_database.cache_size(), 0);

    coin_database.store_to_cache_help(std::move(transaction1));

    EXPECT_EQ(coin_database.cache_size(), 0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    std :: vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std::move(output2));

    std::unique_ptr<Transaction> transaction2 = std::make_unique<Transaction>(
            std :: move(inputs),
            std :: move(outputs2),
            1,
            1);

    coin_database.store_to_cache_help(std::move(transaction2));

    std :: vector<std::unique_ptr<TransactionOutput>> buffer;
    outputs2.push_back(std::move(output2));

    EXPECT_EQ(coin_database.cache_size(), 1);

}

TEST(CoinDatabase, store_transactions_to_main_cache){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    Transaction tran1 = Transaction(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);



    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));

    EXPECT_EQ(coin_database.cache_size(), 0);
    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));

    EXPECT_EQ(coin_database.cache_size(), 2);

}


TEST(CoinDatabase, flush_main_cache){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    Transaction tran1 = Transaction(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);



    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));

    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));
    //check the utxo are stored
    EXPECT_EQ(coin_database.cache_size(), 2);
    coin_database.flush_main_cache();
    EXPECT_EQ(coin_database.cache_size(), 0);
}


TEST(CoinDatabase, return_matching_utxo){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));

    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    Transaction tran1 = Transaction(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);

    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));

    uint32_t trx_hash = RathCrypto::hash(Transaction::serialize(tran1));

    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));

    std::unique_ptr<TransactionInput> new_input = std::make_unique<TransactionInput>(
            trx_hash, 0, 4);

    std::tuple<uint32_t, uint32_t> result = std::tuple(10, 0);

    EXPECT_EQ(coin_database.return_matching_utxo(std::move(new_input)), result);
}


TEST(CoinDatabase, find_index){
CoinDatabase coin_database = CoinDatabase();
    /// The up-to-date indexes of the Transaction's UTXO
    std::vector<uint32_t> utxo {1,2,4,7,9};
    /// the corresponding amounts of the utxo
    std::vector<uint32_t> amounts {10,4,3,8,0};
    /// the corresponding public keys of the utxo
    std::vector<uint32_t> public_keys {1,2,3,4,5};

    const std::unique_ptr<CoinRecord>& coin = std::make_unique<CoinRecord>(0,
                                                                           utxo,
                                                                           amounts,public_keys);
    EXPECT_EQ(coin_database.find_index(0,3,coin), 2);

    uint32_t index = coin_database.find_index(0,3,coin);

    utxo.insert(utxo.begin() + index, 3);
    amounts.insert(amounts.begin() + index, 100);
    public_keys.insert(public_keys.begin() + index, 100);

    /// The up-to-date indexes of the Transaction's UTXO
    std::vector<uint32_t> utxo2 {1,2,3,4,7,9};
    /// the corresponding amounts of the utxo
    std::vector<uint32_t> amounts2 {10,4,100,3,8,0};
    /// the corresponding public keys of the utxo
    std::vector<uint32_t> public_keys2 {1,2,100,3,4,5};


    EXPECT_EQ(utxo, utxo2);
    EXPECT_EQ(amounts, amounts2);
    EXPECT_EQ(public_keys, public_keys2);
}


TEST(CoinDatabase, remove_coin_from_database){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    Transaction tran1 = Transaction(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);

    uint32_t hash = RathCrypto::hash(Transaction::serialize(*transaction1));

    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));


    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 2);

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo[0], 0);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts[0], 10);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys[0], 0);

    std::unique_ptr<CoinLocator> coin = std::make_unique<CoinLocator>(hash, 0);
    coin_database.remove_coin_from_database(std::move(coin));

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo[0], 1);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts[0], 20);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys[0], 1);


    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 1);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 1);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 1);

}


TEST(CoinDatabase, add_utxo){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));


    Transaction tran1 = Transaction(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);

    std::vector<uint32_t> utxo{0,1};
    std::vector<uint32_t> amount{10,20};
    std::vector<uint32_t> public_key{0,1};
    std::unique_ptr<UndoCoinRecord> record = std::make_unique<UndoCoinRecord>(
            1,utxo, amount, public_key);

    uint32_t hash = RathCrypto::hash(Transaction::serialize(tran1));

    coin_database.add_utxo(record, hash);

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 2);



    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo[0], 0);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts[0], 10);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys[0], 0);

    std::vector<uint32_t> utxo2{2};
    std::vector<uint32_t> amount2{30};
    std::vector<uint32_t> public_key2{3};
    std::unique_ptr<UndoCoinRecord> record2 = std::make_unique<UndoCoinRecord>(
            1,utxo2, amount2, public_key2);

    coin_database.add_utxo(record2, hash);

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo[2], 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts[2], 30);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys[2], 3);


    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 3);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 3);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 3);

}


TEST(CoinDatabase, remove_utxo){
    CoinDatabase coin_database = CoinDatabase();
    //empty inputs
    std :: vector<std::unique_ptr<TransactionInput>> inputs;

    //two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

    //two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

    //store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    std::unique_ptr<Transaction> tran1 = std::make_unique<Transaction>(std::move(inputs),
                                    std::move(outputs),
                                    1,
                                    1);

    uint32_t hash = RathCrypto::hash(Transaction::serialize(*transaction1));

    //create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));


    //store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 2);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 2);

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo[0], 0);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts[0], 10);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys[0], 0);

    EXPECT_EQ(coin_database.cache_size(), 2);


    coin_database.remove_utxo(std::move(tran1));

    EXPECT_EQ(coin_database.cache_size(), 0);

    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->utxo.size(), 0);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->amounts.size(), 0);
    EXPECT_EQ(coin_database.get_from_database(std::to_string(hash))->public_keys.size(), 0);

}



TEST(CoinDatabase, validate_transaction){
    CoinDatabase coin_database = CoinDatabase();
//empty inputs


//two outputs
    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);

//store outputs in vector
    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));
    outputs.push_back(std :: move(output2));



    std :: vector<std::unique_ptr<TransactionInput>> inputs2;

//two outputs
    std::unique_ptr<TransactionOutput> output3 = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output4 = std::make_unique<TransactionOutput>(
            20,
            1);

//store outputs in vector
    std::vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output3));
    outputs2.push_back(std :: move(output4));

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            1,
            1);

    uint32_t transaction1_hash = RathCrypto::hash(Transaction::serialize(*transaction1));

//create a vector with one transaction
    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction1));



//store to cache
    coin_database.store_transactions_to_main_cache(std::move(transactions));


    std :: vector<std::unique_ptr<TransactionInput>> inputs;
    std::unique_ptr<TransactionInput> input = std::make_unique<TransactionInput>(
            transaction1_hash,
            0,
            0);

    std::unique_ptr<TransactionInput> input2 = std::make_unique<TransactionInput>(
            transaction1_hash,
            1,
            1);

    inputs.push_back(std::move(input));
    inputs.push_back(std::move(input2));

    std::unique_ptr<Transaction> tran1 = std::make_unique<Transaction>(std::move(inputs),
                                                                       std::move(outputs),
                                                                       1,
                                                                       1);

    std::vector<std::unique_ptr<TransactionInput>> inputs5;
    std::vector<std::unique_ptr<TransactionOutput>> outputs5;
    std::unique_ptr<TransactionInput> input6 = std::make_unique<TransactionInput>(1,2,3);
    std::unique_ptr<TransactionOutput> output6 = std::make_unique<TransactionOutput>(1,2);
    inputs5.push_back(std::move(input6));
    outputs5.push_back(std::move(output6));

    std::unique_ptr<Transaction> tran2 = std::make_unique<Transaction>(std::move(inputs5),
                                                                      std::move(outputs5),
                                                                      1,
                                                                      1);

    EXPECT_TRUE(coin_database.validate_transaction(std::move(tran1)));


}


