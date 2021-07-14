#include <chain_writer.h>
#include <rathcrypto.h>

const std::string ChainWriter::_file_extension = "data";
const std::string ChainWriter::_block_filename = "blocks";
const uint16_t ChainWriter::_max_block_file_size = 1000;
const std::string ChainWriter::_undo_filename = "undo_blocks";
const uint16_t ChainWriter::_max_undo_file_size = 1000;
const std::string ChainWriter::_data_directory = "data";

ChainWriter::ChainWriter():
_current_block_file_number(0),
_current_block_offset(0),
_current_undo_file_number(0),
_current_undo_offset(0){}

//ToDO: " Note that block is a constant reference (you cannot modify it).
// This means that you will have to copy its block_header when creating
// the BlockRecord. You do not want to use std::move here."

std::unique_ptr<BlockRecord> ChainWriter :: store_block(const Block& block, uint32_t height){
    //make undo block
    std::vector<uint32_t> transaction_hashes;
    std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;

    for( int a = 0; a < block.transactions.size(); a = a + 1 ) {
        uint32_t trx_hash = RathCrypto::hash(Transaction ::serialize(*block.transactions[a]));
        transaction_hashes.insert(transaction_hashes.end(), trx_hash);
        std::vector<uint32_t> utxo;
        /// the corresponding amounts of the utxo
        std::vector<uint32_t> amounts;
        /// the corresponding public keys of the utxo
        std::vector<uint32_t> public_keys;


        for ( int b = 0; b < block.transactions[a]->transaction_inputs.size(); b = b + 1 ) {
            const std::unique_ptr<TransactionInput>& trx_in = block.transactions[a]->transaction_inputs[b];
            std:: tuple<uint32_t, uint32_t> utxo = CoinDatabase::return_matching_utxo(&trx_in);

            utxo.insert(utxo.end(),1,b); //place at end or beginning?
            amounts.insert(amounts.end(),1, trx_in-> ;//ToDO: ??);
            public_keys.insert(public_keys.end(), 1, trx_in->signature);
            }  //ToDo: is public key signature?

        }
            block.transactions[a]->transaction_inputs[b]
    }
    std::unique_ptr<UndoBlock> undo_block = std::make_unique<UndoBlock>(
            //transaction outputs --> convert
            //          input (reference transaction hash, utxo index, signature)
            //          to output (amount, public key)


            //undo_coin_records: //utxo, amount, public keys
            )
    //make vector <block, undo block>
    //call write block, get File Info
    //create and return block Record
}



std::vector<std::unique_ptr<FileInfo>> ChainWriter::write_block( std::vector<std::string> blocks) {
    FILE *pFile;
    std::vector<std::unique_ptr<FileInfo>> files;

    for( int a = 0; a < blocks.size(); a = a + 1 ) {
        int n = blocks[a].length();
        char char_block[n + 1];
        strcpy(char_block, blocks[a].c_str());


        //ToDo: can we assume block and undo block have same max block size
        if (_current_block_offset + n > get_max_block_file_size()) { //check for current size of file{
            _current_block_offset = 0;
            _current_block_file_number = +1;
        }
        //ToDo: should we use the getters?

        pFile = fopen(ChainWriter::get_filename().c_str(), "a");
        fwrite(char_block, sizeof(char), sizeof(char_block), pFile);
        fclose(pFile);

        uint16_t new_offset = _current_block_offset + n;
        std::unique_ptr<FileInfo> info = std::make_unique<FileInfo>(
                get_filename(),
                _current_block_offset,
                new_offset);

        _current_block_offset = new_offset;

        files.insert(files.end(), std::move(info));

    }
    //ToDo: check for file size, and open new file accordingly
    return files;
}

std::string ChainWriter::get_filename() {
    std::string filename = _data_directory + "/"
                           + _block_filename + "_"
                           + std::to_string(_current_block_file_number)
                           + _file_extension;
    return filename;
}

