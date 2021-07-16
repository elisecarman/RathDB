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
#include <chain.h>
#include<stdio.h>

TEST(ChainWriter, write_block) {
    ChainWriter chain_writer = ChainWriter();
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
    transaction_inputs.push_back(std::make_unique<TransactionInput>(
            0,
            1,
            99
    ));

    std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
    transaction_outputs.push_back(std::make_unique<TransactionOutput>(10,1));
    transactions.push_back(std::make_unique<Transaction>(std::move(transaction_inputs),
                                                         std::move(transaction_outputs)));
    std::unique_ptr<BlockHeader> block_header =
            std::make_unique<BlockHeader>(
                    0,
                    2,
                    3,
                    4,
                    5,
                    6);
    std::unique_ptr<Block> b = std::make_unique<Block>( std::move(block_header), std::move(transactions));

    std::unique_ptr<FileInfo> info = chain_writer.write_block(Block::serialize(*b));

    std::string read_block = chain_writer.read_block(*info);

    EXPECT_EQ(Block::deserialize(read_block), b);

}