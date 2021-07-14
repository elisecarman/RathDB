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





TEST(CoinDatabase, store_transactions_to_main_cache){

           std::unique_ptr<TransactionInput> input = std::make_unique<TransactionInput>(
            0,
            1,
            2);
           std::unique_ptr<TransactionInput> input2 = std::make_unique<TransactionInput>(
            3,
            4,
            5);


    std::unique_ptr<TransactionOutput> output = std::make_unique<TransactionOutput>(
            10,
            0);

    std::unique_ptr<TransactionOutput> output2 = std::make_unique<TransactionOutput>(
            20,
            1);


    std :: vector<std::unique_ptr<TransactionInput>> inputs;
    inputs.push_back(std :: move(input));

    std :: vector<std::unique_ptr<TransactionOutput>> outputs;
    outputs.push_back(std :: move(output));

    std :: vector<std::unique_ptr<TransactionInput>> inputs2;
    inputs2.push_back(std :: move(input2));

    std :: vector<std::unique_ptr<TransactionOutput>> outputs2;
    outputs2.push_back(std :: move(output2));

//    Transaction transaction1 = Transaction(
//            std :: move(inputs),
//            std :: move(outputs),
//            0,
//            0);

    std::unique_ptr<Transaction> transaction1 = std::make_unique<Transaction>(
            std :: move(inputs),
            std :: move(outputs),
            1,
            1);

    std::unique_ptr<Transaction> transaction2 = std::make_unique<Transaction>(
            std :: move(inputs2),
            std :: move(outputs2),
            0,
            0);


    std :: vector<std :: unique_ptr<Transaction>> transactions;
    transactions.push_back(std :: move(transaction1));
    transactions.push_back(std :: move(transaction2));

    std::vector<CoinRecord> records = CoinDatabase :: store_transactions_to_main_cache(
            std::move(transactions));


    std :: vector<uint32_t> utxo{1};
    std :: vector<uint32_t> amounts{10};
    std :: vector<uint32_t> pubKeys{0};

    CoinRecord coin_record = CoinRecord(1, std::move(utxo), std::move(amounts),
                                        std::move(pubKeys));
    std :: vector<uint32_t> utxo2{4};
    std :: vector<uint32_t> amounts2{20};
    std :: vector<uint32_t> pubKeys2{1};

    CoinRecord coin_record2 = CoinRecord(0, std::move(utxo2), std::move(amounts2),
                                        std::move(pubKeys2));

    std ::vector<CoinRecord> records2;
    records2.push_back(std::move(coin_record));
    records2.push_back(std::move(coin_record2));

    EXPECT_EQ(records, records2);

    //<1>,<4> utxo index
//<10>,<20> amounts
//<0>,<1> pubkeys

}