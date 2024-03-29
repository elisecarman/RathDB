/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <gtest/gtest.h>
#include <chain_writer.h>
#include <chain.h>
#include <rathcrypto.h>
#include <filesystem>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

TEST(Chain, GetLastBlock){
    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs =
            genesis_block->transactions[0]->transaction_outputs;
    EXPECT_EQ(genesis_block->transactions.size(), 1);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);
}

///free autograder test
TEST(Chain, GetGenesisBlockHash0) {
// test setup - remove past data
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();
// check that genesis block has at least 1 transaction
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);
    const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs =
            genesis_block->transactions.at(0)->transaction_outputs;
// check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);

    EXPECT_EQ(Block::serialize(*genesis_block), chain.return_string());

    uint32_t hash0 = RathCrypto::hash(Block::serialize(*genesis_block));
    EXPECT_EQ(hash0, chain.get_last_block_hash());

    std::filesystem::remove_all(ChainWriter::get_data_directory());
}

std::unique_ptr<Block> make_blockd(std::unique_ptr<Block> block, int tx_index, int out_index) {
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
    transaction_inputs.push_back(std::make_unique<TransactionInput>(
            RathCrypto::hash(Transaction::serialize(*block->transactions.at(tx_index))),
            out_index,
            99));
    std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
    transaction_outputs.push_back(std::make_unique<TransactionOutput>(
            block->transactions.at(tx_index)->transaction_outputs.at(out_index)->amount,
            block->transactions.at(tx_index)->transaction_outputs.at(out_index)->public_key
    ));
    transactions.push_back(std::make_unique<Transaction>(
            std::move(transaction_inputs), std::move(transaction_outputs)));

    std::unique_ptr<BlockHeader> block_header =std::make_unique<BlockHeader>(
            1,
            RathCrypto::hash(Block::serialize(*block)),
            3,
            4,
            5,
            6);
    std::unique_ptr<Block> b = std::make_unique<Block>(
            std::move(block_header), std::move(transactions));
    return std::move(b);
}


///free autograder test
TEST(Chain, HandleValidBlock23) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    // check that genesis block has at least 1 transaction
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);
    const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs = genesis_block->transactions.at(
            0)->transaction_outputs;
    // check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);
    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));
    chain.handle_block(std::move(block));

    EXPECT_EQ(block_hash, chain.get_last_block_hash());
    std::unique_ptr<Block> ret_block = chain.get_last_block();
    EXPECT_EQ(ret_block->transactions.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);
}


TEST(Chain, GetActiveChainLength) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> block = make_blockd(chain.get_last_block(), 0, 0);

    chain.handle_block(std::move(block));

    uint32_t length = chain.get_active_chain_length();

    EXPECT_TRUE(length == 2);
}


//tests the get block function on the second block
TEST(Chain, GetBlock) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();

    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));

    chain.handle_block(std::move(block));

    std::unique_ptr<Block> ret_block = chain.get_block(block_hash);

    EXPECT_EQ(ret_block->transactions.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);


}


//tests a block that doesnt exist in chain
TEST(Chain, GetBlockNotExist){
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();

    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));

    std::unique_ptr<Block> b2 = chain.get_block(block_hash);

    EXPECT_TRUE(b2 == nullptr );
}


//test getting active chain of one block
TEST(Chain, GetActiveChain1) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());
    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();

    std::vector<std::unique_ptr<Block>> active_chain = chain.get_active_chain(1, 1);

    EXPECT_EQ(active_chain.size(), 1);


    EXPECT_EQ(active_chain.at(0)->block_header->previous_block_hash,
              chain.get_last_block()->block_header->previous_block_hash);
    EXPECT_EQ(active_chain.at(0)->block_header->nonce,
              chain.get_last_block()->block_header->nonce);
    EXPECT_EQ(active_chain.at(0)->block_header->difficulty_target,
              chain.get_last_block()->block_header->difficulty_target);
    EXPECT_EQ(active_chain.at(0)->block_header->merkle_root,
              chain.get_last_block()->block_header->merkle_root);
    EXPECT_EQ(active_chain.at(0)->transactions[0]->transaction_outputs.size(),
              chain.get_last_block()->transactions[0]->transaction_outputs.size());
}

//gets active chain of two blocks
TEST(Chain, GetActiveChain2) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    // 1 = add valid to gaenesis
    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    // check that genesis block has at least 1 transaction

    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));
    chain.handle_block(std::move(block));

    std::vector<std::unique_ptr<Block>> active_chain = chain.get_active_chain(1, 2);

    EXPECT_EQ(active_chain.size(), 2);
    //checks if blocks match
    EXPECT_EQ(active_chain.at(1)->block_header->previous_block_hash,
              chain.get_last_block()->block_header->previous_block_hash);
    EXPECT_EQ(active_chain.at(1)->block_header->nonce,
              chain.get_last_block()->block_header->nonce);
    EXPECT_EQ(active_chain.at(1)->block_header->difficulty_target,
              chain.get_last_block()->block_header->difficulty_target);
    EXPECT_EQ(active_chain.at(1)->block_header->merkle_root,
              chain.get_last_block()->block_header->merkle_root);
    EXPECT_EQ(active_chain.at(1)->transactions[0]->transaction_outputs.size(),
              chain.get_last_block()->transactions[0]->transaction_outputs.size());

}

//same as getActiveChain1 but with hashes
TEST(Chain, GetActiveChainHashes1) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    // 1 = add valid to genesis
    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    // check that genesis block has at least 1 transaction

    std::vector<uint32_t> active_chain_hashes = chain.get_active_chain_hashes(1, 1);

    EXPECT_EQ(active_chain_hashes.size(), 1);
    EXPECT_EQ(active_chain_hashes.at(0), RathCrypto::hash(Block::serialize(*genesis_block)));
}

//tests getting undoblock of genesis
TEST(Chain, TestGetUndoBlock1) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());
    Chain chain = Chain();
    std::unique_ptr<Block> genesis_block = chain.get_last_block();

    //get undo block for genesis block
    uint32_t hash = RathCrypto::hash(Block::serialize(*genesis_block));
    std::unique_ptr<UndoBlock> undo_block = chain.get_undo_block(hash);

    //transaction hash of first gen block transaction (which should be the hash of the first trx in undo block
    uint32_t trx_hash = RathCrypto::hash(Transaction::serialize(
            *genesis_block->transactions.at(0)));

    EXPECT_EQ(undo_block->transaction_hashes[0], trx_hash);

}

//test handle chain of length 3
TEST(Chain, HandleBlock34) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    // 1 = add valid to genesis
    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    // check that genesis block has at least 1 transaction
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);
    const std::vector<std::unique_ptr<TransactionOutput>>& transaction_outputs =
            genesis_block->transactions.at(0)->transaction_outputs;
    // check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);
    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);

    chain.handle_block(std::move(block));
    //append third block
    std::unique_ptr<Block> block3 = make_blockd(chain.get_last_block(), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block3));

    chain.handle_block(std::move(block3));

    EXPECT_EQ(block_hash, chain.get_last_block_hash());
    std::unique_ptr<Block> ret_block = chain.get_last_block();
    EXPECT_EQ(ret_block->transactions.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);

    EXPECT_EQ(chain.get_active_chain_length(), 3);
}

//tests adding two blocks attached to genesis
TEST(Chain, TwoBranches) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    // 1 = add valid to genesis
    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    std::unique_ptr<Block> genesis_block1 = chain.get_last_block();

    // check that genesis block has at least 1 transaction
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);
    const std::vector<std::unique_ptr<TransactionOutput>>& transaction_outputs =
            genesis_block->transactions.at(0)->transaction_outputs;

    // check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);

    //makes and handles second block
    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));
    chain.handle_block(std::move(block));

    //makes second branch
    std::unique_ptr<Block> block2 = make_blockd(std::move(genesis_block1), 0, 1);
    uint32_t block_hash2 = RathCrypto::hash(Block::serialize(*block2));
    chain.handle_block(std::move(block2));


    EXPECT_EQ(chain.get_active_chain_length(), 2);
    EXPECT_EQ(block_hash, chain.get_last_block_hash());
    std::unique_ptr<Block> ret_block = chain.get_last_block();
    EXPECT_EQ(ret_block->transactions.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);
}

//tests a fork that occurs
TEST(Chain, ForkInTheRoad) {
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    // 1 = add valid to genesis
    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    std::unique_ptr<Block> genesis_block1 = chain.get_last_block();

    // check that genesis block has at least 1 transaction
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);
    const std::vector<std::unique_ptr<TransactionOutput>>& transaction_outputs =
            genesis_block->transactions.at(0)->transaction_outputs;

    // check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);

    //makes and handles second block
    std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
    uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));
    chain.handle_block(std::move(block));

    //makes second branch
    std::unique_ptr<Block> fork1 = make_blockd(std::move(genesis_block1), 0, 1);
    uint32_t block_hash2 = RathCrypto::hash(Block::serialize(*fork1));
    chain.handle_block(std::move(fork1));

    std::unique_ptr<Block> get_fork_1 = chain.get_block(block_hash2);
    std::unique_ptr<Block> fork2 = make_blockd(std::move(get_fork_1), 0, 0);
    uint32_t block_hash3 = RathCrypto::hash(Block::serialize(*fork2));
    chain.handle_block(std::move(fork2));


    EXPECT_EQ(chain.get_active_chain_length(), 3);

    EXPECT_EQ(block_hash3, chain.get_last_block_hash());
    std::unique_ptr<Block> ret_block = chain.get_last_block();
    EXPECT_EQ(ret_block->transactions.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
    EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 200);
}