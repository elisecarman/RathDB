#include <chain.h>
#include <block_info_database.h>
#include <block.h>
#include <undo_block.h>
#include <coin_database.h>
#include <block_header.h>
#include <block_record.h>
#include <chain_writer.h>
#include <rathcrypto.h>

//--

std::unique_ptr<UndoBlock> Chain:: make_undo_block(std::unique_ptr<Block> block) {
    //get transactions
    std::vector<uint32_t> transaction_hashes;
    std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;
    std::vector<std::unique_ptr<Transaction>> block_trx = block->get_transactions();

    for (int a = 0; a < block_trx.size(); a = a + 1) {
        Transaction trx = *block_trx[a];
        transaction_hashes.insert(transaction_hashes.end(), 1,
                                  RathCrypto::hash(Transaction::serialize(trx)));

        std::vector<uint32_t> utxo;
        std::vector<uint32_t> amounts;
        std::vector<uint32_t> public_keys;

        for (int b = 0; b < trx.transaction_inputs.size(); b = b + 1) {
            std::tuple<uint32_t ,uint32_t > tup = _coin_database->return_matching_utxo(
                    std::move(trx.transaction_inputs[a]));
            utxo.insert(utxo.end(), 1, trx.transaction_inputs[a]->utxo_index);
            amounts.insert(amounts.end(), 1, std::get<0>(tup));
            public_keys.insert(public_keys.end(), 1,  std::get<1>(tup));
        }

        std:: unique_ptr<UndoCoinRecord> record = std::make_unique<UndoCoinRecord>(
                trx.version, utxo, amounts, public_keys);

        undo_coin_records.insert(undo_coin_records.end(), std::move(record));
        //get transaction inputs
        //-> fill the coin record vector
        //-> fill the transaction hash vector
    }
    std::unique_ptr<UndoBlock> u_block = std::make_unique<UndoBlock>(transaction_hashes, std::move(undo_coin_records));

    return u_block;
}

std::unique_ptr<Block> Chain::construct_genesis_block() {
    //construct a new block and add it store it into the database
    //random recipients
    std::vector<std::unique_ptr<TransactionInput>> _inputs;
    std::vector<std::unique_ptr<TransactionOutput>> _outputs;
    std::vector<std::unique_ptr<Transaction>> _trans;

    std::unique_ptr<TransactionInput> input = std::make_unique<TransactionInput>(0,
                                                                                 0,
                                                                                 0);
    std::unique_ptr<TransactionOutput> output_1 = std::make_unique<TransactionOutput>(100, 12345);
    std::unique_ptr<TransactionOutput> output_2 = std::make_unique<TransactionOutput>(200, 67891);
    std::unique_ptr<TransactionOutput> output_3 = std::make_unique<TransactionOutput>(300, 23456);
    _outputs.push_back(std::move(output_1));
    _outputs.push_back(std::move(output_2));
    _outputs.push_back(std::move(output_3));
    _inputs.push_back(std::move(input));
    //check if lock time is right

    std::unique_ptr<Transaction> trans = std::make_unique<Transaction>(
            std::move(_inputs),
            std::move(_outputs),
            1,
            1);
    _trans.push_back(std::move(trans));

    std::unique_ptr<BlockHeader> gen_block_header = std::make_unique<BlockHeader>(1,
                                                                                  0,
                                                                                  0,
                                                                                  0,
                                                                                  0,
                                                                                  0);
    std::unique_ptr<Block> genesis = std::make_unique<Block>(std::move(gen_block_header), std::move(_trans));
    return genesis;
}



Chain::Chain() : _active_chain_length(1), _active_chain_last_block(construct_genesis_block()),
                 _block_info_database(std::make_unique<BlockInfoDatabase>()),
                 _chain_writer(std::make_unique<ChainWriter>()), _coin_database(std::make_unique<CoinDatabase>())
{

    //can't use move i don't think? might need to change undo block function parameters
    //std::unique_ptr<UndoBlock> undo_block = make_undo_block(std::move(_active_chain_last_block));


    _chain_writer->write_block(Block::serialize(*_active_chain_last_block));
    //_chain_writer->write_undo_block(UndoBlock::serialize(*undo_block));

    std::unique_ptr<UndoBlock> undo = Chain::make_undo_block(std::move(_active_chain_last_block));

    std::unique_ptr<BlockRecord> block_record = _chain_writer->store_block(*_active_chain_last_block,
                                                                           *undo, 1);


    _block_info_database->store_block_record(RathCrypto::hash(BlockRecord::serialize(*block_record)), *block_record);
    _coin_database->store_block(_active_chain_last_block->get_transactions());

    _last_five_hashes.push_back(RathCrypto::hash(Block::serialize(*_active_chain_last_block)));

}

void Chain::handle_block(std::unique_ptr<Block> block) {
    bool valid;
    if (block->block_header->previous_block_hash == get_last_block_hash()) {
        valid = _coin_database->validate_and_store_block(block->get_transactions());

        _active_chain_length += 1;
        _active_chain_last_block = std::move(block);

        //not sure how to properly keep track of last 5 hashes
        if (_last_five_hashes.size() < 5) {
            _last_five_hashes.push_back(RathCrypto::hash(Block::serialize(*_active_chain_last_block)));
        }
        else {
            _last_five_hashes.erase(_last_five_hashes.begin());
            _last_five_hashes.push_back(RathCrypto::hash(Block::serialize(*_active_chain_last_block)));
        }
    }

    else {
        valid = _coin_database->validate_block(block->get_transactions());
    }

    if (valid) {

        uint32_t prev_hash = block->block_header->previous_block_hash;
        std::unique_ptr<BlockRecord> prev_record = _block_info_database->get_block_record(prev_hash);

        std::unique_ptr<UndoBlock> undo_block = make_undo_block(std::move(block));
        std::unique_ptr<BlockRecord> rec = _chain_writer->store_block(*block, *undo_block,
                                                                      prev_record->height + 1);
        _chain_writer->write_block(Block::serialize(*block));
        std::unique_ptr<UndoBlock> undo = make_undo_block(std::move(block));
        _chain_writer->write_undo_block(UndoBlock::serialize(*undo));

        _block_info_database->store_block_record(RathCrypto::hash(Block::serialize(*block)), *rec);


        //if a fork occurs
        if (rec->height > _active_chain_length) {
            std::vector<std::shared_ptr<Block>> forked_stack = get_forked_blocks_stack(prev_hash);

            //need to find branching height
            std::vector<std::unique_ptr<UndoBlock>> undo_queue = get_undo_blocks_queue(rec->height);

            std::vector<std::unique_ptr<Block> active_chain = Chain::get_active_chain()
                    //ToDo: get active chain
            _coin_database->undo_coins(undo_queue, );
            for (int i = 0; i < undo_queue.size(); i++) {
                std::shared_ptr<Block> fork = forked_stack[i];
                _coin_database->store_block(fork->get_transactions());
            }
        }
    }



}


void Chain::handle_transaction(std::unique_ptr<Transaction> transaction) {
    _coin_database->validate_and_store_transaction(std::move(transaction));
}

//--
uint32_t Chain::get_chain_length(uint32_t block_hash) {
    std::unique_ptr<BlockRecord> block_record = _block_info_database->get_block_record(block_hash);
    return block_record->height;
    //info is readily availble in block records
}

//--
std::unique_ptr<Block> Chain::get_block(uint32_t block_hash) {
    std::unique_ptr<BlockRecord> block_record = _block_info_database->get_block_record(block_hash);
    FileInfo file_info = FileInfo(block_record->block_file_stored, block_record->block_offset_start, block_record->block_offset_end);
    std::string read_block = _chain_writer->read_block(file_info);
    return Block::deserialize(read_block);
}

//gets undo blocks
std::unique_ptr<UndoBlock> Chain::get_undo_block(uint32_t block_hash) {
    //blocks are stored on disk, block info database stores location
    //query block info database to find where block is stored on disk  , then use chain writer to read from disk
    std::unique_ptr<BlockRecord> block_record = _block_info_database->get_block_record(block_hash);
    FileInfo file_info = FileInfo(block_record->undo_file_stored,
                                  block_record->undo_offset_start, block_record->undo_offset_end);
    std::string read_undo_block = _chain_writer->read_undo_block(file_info);
    return UndoBlock::deserialize(read_undo_block);
}


std::vector<std::unique_ptr<Block>> Chain::get_active_chain(uint32_t start, uint32_t end) {
    if (end < start) {
        //not sure if returing empty vector right
        std::vector<std::unique_ptr<Block>> hi;
        return hi;
    }
    if (start == 0) {
        start = 1;
    }
    std::unique_ptr<BlockRecord> prev_record = _block_info_database->get_block_record(get_last_block_hash());
    std::vector<std::unique_ptr<Block>> _active_chain;
    uint32_t height = prev_record->height;
    std::unique_ptr<Block> block;
    if (prev_record->height >= start && prev_record->height <= end) {
        FileInfo file = FileInfo(prev_record->block_file_stored, prev_record->block_offset_start, prev_record->block_offset_end);
        std::string read_block = _chain_writer->read_block(file);
        block = Block::deserialize(read_block);
        _active_chain.push_back(std::move(block));
    }

    while (height >= start) {
        uint32_t prevB_hash = block->block_header->previous_block_hash;
        std::unique_ptr<Block> prev_block = get_block(prevB_hash);
        block = std::move(prev_block);
        _active_chain.push_back(std::move(block));
        height -= 1;
    }

    std::reverse(_active_chain.begin(), _active_chain.end());
    //need to slice vector
    auto first = _active_chain.begin();
    auto last = _active_chain.end();



    return Chain::reverse_vector(std::move(_active_chain));
    // need to return vector with correct end
}

std::vector<std::unique_ptr<Block>> Chain:: reverse_vector(std::vector<std::unique_ptr<Block>> vector){
    for( int a = 0; a < vector.size(); a = a + 1 ) {
        vector.push_back(std::move(vector[vector.size()-a]));
    }
}


std::vector<uint32_t> Chain::get_active_chain_hashes(uint32_t start, uint32_t end) {
    std::vector<uint32_t> _active_chain_hashes;
    uint32_t hash = get_last_block_hash();
    _active_chain_hashes.push_back(hash);
    std::unique_ptr<Block> block = get_last_block();
    uint32_t height = _active_chain_length - 1;
    while (height >= start) {
        uint32_t prevB_hash = block->block_header->previous_block_hash;
        _active_chain_hashes.push_back(prevB_hash);
        block = get_block(prevB_hash);
        height -= 1;
    }

    std::reverse(_active_chain_hashes.begin(), _active_chain_hashes.end());
    auto first = _active_chain_hashes[0];
    auto last = _active_chain_hashes[end];

    std::vector<uint32_t> vector(first, last);

    return vector;
}



std::unique_ptr<Block> Chain::get_last_block() {

    std::vector<std::unique_ptr<Transaction>> last_block_trans;

    std::unique_ptr<BlockHeader> header;

    header->set_difficulty_target(_active_chain_last_block->block_header->difficulty_target);
    header->set_merkle_root(_active_chain_last_block->block_header->merkle_root);
    header->set_nonce(_active_chain_last_block->block_header->nonce);
    header->set_previous_block_hash(_active_chain_last_block->block_header->previous_block_hash);
    header->set_timestamp(_active_chain_last_block->block_header->timestamp);
    header->set_version(_active_chain_last_block->block_header->version);

    for (int i = 0; i < _active_chain_last_block->transactions.size(); i++) {

        std::vector<std::unique_ptr<TransactionInput>> trans_in;
        std::vector<std::unique_ptr<TransactionOutput>> trans_out;

        for (int j = 0; j < _active_chain_last_block->transactions[i]->transaction_inputs.size(); j++) {
            std::unique_ptr<TransactionInput> txi = std::make_unique<TransactionInput>(_active_chain_last_block->transactions[i]->transaction_inputs[j]->reference_transaction_hash,
                                                                                       _active_chain_last_block->transactions[i]->transaction_inputs[j]->utxo_index, _active_chain_last_block->transactions[i]->transaction_inputs[j]->signature);
            trans_in.push_back(std::move(txi));
        }
        for (int k = 0; k < _active_chain_last_block->transactions[i]->transaction_outputs.size(); k++) {
            std::unique_ptr<TransactionOutput> txo = std::make_unique<TransactionOutput>(_active_chain_last_block->transactions[i]->transaction_outputs[k]->amount,
                                                                                         _active_chain_last_block->transactions[i]->transaction_outputs[k]->public_key);
            trans_out.push_back(std::move(txo));
        }
        std::unique_ptr<Transaction> new_trans = std::make_unique<Transaction>(std::move(trans_in), std::move(trans_out),
                                                                               _active_chain_last_block->transactions[i]->version, _active_chain_last_block->transactions[i]->lock_time);

        last_block_trans.push_back(std::move(new_trans));

    }

    std::unique_ptr<Block> last_block = std::make_unique<Block>(std::move(header), std::move(last_block_trans));
    return last_block;
}

uint32_t Chain::get_last_block_hash() {
    return RathCrypto::hash(Block::serialize(*_active_chain_last_block));
}

uint32_t Chain::get_active_chain_length() const {
    return _active_chain_length;
}

std::vector<std::pair<uint32_t, uint8_t>> Chain::get_all_utxo(uint32_t public_key) {
    _coin_database->flush_main_cache();
    return _coin_database->get_all_utxo(public_key);
}

std::vector<std::unique_ptr<UndoBlock>> Chain::get_undo_blocks_queue(uint32_t branching_height) {
    std::unique_ptr<UndoBlock> undo_block = get_undo_block(get_last_block_hash());
    std::vector<std::unique_ptr<UndoBlock>> queue;
    queue.push_back(std::move(undo_block));
    std::unique_ptr<Block> block = get_last_block();
    int height = _active_chain_length;
    uint32_t prev_hash = block->block_header->previous_block_hash;
    //may need height -1
    while (height >= branching_height) {

        std::unique_ptr<UndoBlock> prev_undo_block = get_undo_block(prev_hash);
        queue.push_back(std::move(prev_undo_block));
        block = get_block(prev_hash);
        prev_hash = block->block_header->previous_block_hash;
        height -= 1;
    }
    return queue;
}

std::vector<std::shared_ptr<Block>> Chain::get_forked_blocks_stack(uint32_t starting_hash) {
    std::vector<std::shared_ptr<Block>> forked_stack;
    std::shared_ptr<Block> block = get_block(starting_hash);
    //adds starting block to stack
    uint32_t prev_hash = block->block_header->previous_block_hash;
    forked_stack.push_back(std::move(block));

    bool is_common_anc = false;
    int branching_height;

    while (!is_common_anc) {
        //loop through last five hashes and check if active branch (common ancestor) has been reached
        for (int i = 5; i > 0; i--) {
            if (prev_hash == _last_five_hashes[i]) {
                is_common_anc = true;
                branching_height = _block_info_database->get_block_record(prev_hash)->height;
                return forked_stack;

            }

            std::shared_ptr<Block> prev_block = get_block(prev_hash);
            block = std::move(prev_block);
            forked_stack.push_back(std::move(block));
            prev_hash = block->block_header->previous_block_hash;

        }



    }



}

