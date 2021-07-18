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


    EXPECT_EQ(Block::deserialize(read_block)->block_header->difficulty_target, b->block_header->difficulty_target);

    EXPECT_EQ(Block::deserialize(read_block)->block_header->merkle_root, b->block_header->merkle_root);

    EXPECT_EQ(Block::deserialize(read_block)->block_header->nonce, b->block_header->nonce);

    EXPECT_EQ(Block::deserialize(read_block)->block_header->previous_block_hash, b->block_header->previous_block_hash);

    EXPECT_EQ(Block::deserialize(read_block)->transactions.size(), b->transactions.size());

    EXPECT_EQ(Block::deserialize(read_block)->transactions[0]->transaction_outputs.size(), b->transactions[0]->transaction_outputs.size());

    EXPECT_EQ(read_block, Block::serialize(*b)); ////<--------

    std::vector<std::unique_ptr<Transaction>> transactions2;
    std::vector<std::unique_ptr<TransactionInput>> transaction_inputs2;
    transaction_inputs2.push_back(std::make_unique<TransactionInput>(
            0,
            1,
            99
    ));

    std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs2;
    transaction_outputs2.push_back(std::make_unique<TransactionOutput>(10,1));
    transactions2.push_back(std::make_unique<Transaction>(std::move(transaction_inputs2),
                                                         std::move(transaction_outputs2)));
    std::unique_ptr<BlockHeader> block_header2 =
            std::make_unique<BlockHeader>(
                    0,
                    2,
                    3,
                    4,
                    5,
                    6);

    std::unique_ptr<Block> b2 = std::make_unique<Block>( std::move(block_header2), std::move(transactions2));

    std::unique_ptr<FileInfo> info2 = chain_writer.write_block(Block::serialize(*b2));

    std::string read_block2 = chain_writer.read_block(*info2);

    std::unique_ptr<Block> b2_2 = Block::deserialize(read_block2);

    EXPECT_EQ(Block::deserialize(read_block2)->block_header->difficulty_target, b2->block_header->difficulty_target);

    EXPECT_EQ(Block::deserialize(read_block2)->block_header->merkle_root, b2->block_header->merkle_root);

    EXPECT_EQ(Block::deserialize(read_block2)->block_header->nonce, b2->block_header->nonce);

    EXPECT_EQ(Block::deserialize(read_block2)->block_header->previous_block_hash, b2->block_header->previous_block_hash);

    EXPECT_EQ(Block::deserialize(read_block2)->transactions.size(), b2->transactions.size());


    EXPECT_EQ(Block::deserialize(read_block)->transactions[0]->transaction_outputs.size(), b->transactions[0]->transaction_outputs.size());

    EXPECT_EQ(Block::deserialize(read_block)->transactions[0]->transaction_inputs.size(), b->transactions[0]->transaction_inputs.size());

   // EXPECT_EQ(read_block2, Block::serialize(*b2)); ////<--------

}