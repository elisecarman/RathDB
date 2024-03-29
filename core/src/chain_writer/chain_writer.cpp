#include <chain_writer.h>
#include <rathcrypto.h>
#include<stdio.h>
#include <filesystem>

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

std::unique_ptr<BlockRecord> ChainWriter :: store_block(const Block& block,
                                                        const UndoBlock& undo_block,
                                                        uint32_t height){


    std::unique_ptr<FileInfo> block_info = ChainWriter::write_block(
            Block::serialize(block));

    std::unique_ptr<FileInfo> undo_info = ChainWriter::write_undo_block(
            UndoBlock::serialize(undo_block));

    uint32_t trx_size = block.transactions.size();
    std::unique_ptr<BlockHeader> h = ChainWriter::get_header(block);

            return std::make_unique<BlockRecord>(std::move(h),
                                                 trx_size,
                                                 height,
                                                 *block_info,
                                                 *undo_info);

}

std::unique_ptr<BlockHeader> ChainWriter::get_header(const Block& block){

    std::unique_ptr<BlockHeader> h = std::make_unique<BlockHeader>(
            block.block_header->version,
            block.block_header->previous_block_hash,
            block.block_header->merkle_root,
            block.block_header->difficulty_target,
            block.block_header->nonce,
            block.block_header->timestamp);

    return h;
}


std::unique_ptr<FileInfo> ChainWriter::write_block( std::string block) {

    FILE *pFile;
    int n = block.length();
    char char_block[n + 1];
    strcpy(char_block, block.c_str());

    //ToDo: can we assume block and undo block have same max block size
    if (_current_block_offset + n > get_max_undo_file_size()) {
        _current_block_offset = 0;
        _current_block_file_number += 1;
    }

    pFile = fopen(ChainWriter::get_filename(false).c_str(), "at");
    if (!std::filesystem::is_directory(get_data_directory())){
        std::filesystem::create_directory(get_data_directory());
    }
    if (pFile == NULL){
        pFile = fopen(ChainWriter::get_filename(false).c_str(), "wt");
    }

    fwrite(char_block, sizeof(char), sizeof(char_block), pFile);
    fclose(pFile);
    uint16_t new_offset = _current_block_offset + n + 1;
    uint16_t old_block_offset = _current_block_offset;
    _current_block_offset = new_offset;
    return std::make_unique<FileInfo>(
            get_filename(false),
            old_block_offset,
            old_block_offset + n);
}


std::unique_ptr<FileInfo> ChainWriter::write_undo_block( std::string block) {

    FILE *pFile;
    int n = block.length();
    char char_block[n + 1];
    strcpy(char_block, block.c_str());


    if (_current_undo_offset + n > get_max_undo_file_size()) {
        _current_undo_offset = 0;
        _current_undo_file_number += 1;
    }


    pFile = fopen(ChainWriter::get_filename(true).c_str(), "at");
    if (!std::filesystem::is_directory(get_data_directory())){
        std::filesystem::create_directory(get_data_directory());
    }
    if (pFile == NULL){
        pFile = fopen(ChainWriter::get_filename(true).c_str(), "wt");
    }

    fwrite(char_block, sizeof(char), sizeof(char_block), pFile);
    fclose(pFile);

    uint16_t new_offset = _current_undo_offset + n + 1;
    uint16_t old_undo_offset = _current_undo_offset;
    _current_undo_offset = new_offset;
    return std::make_unique<FileInfo>(
            get_filename(true),
            old_undo_offset,
            old_undo_offset + n);
}




std::string ChainWriter::get_filename(bool is_undo) {
    if (is_undo) {
        return _data_directory + "/"
                                + _undo_filename + "_"
                                + std::to_string(_current_undo_file_number) + "."
                                + _file_extension;

    } ///seem to return the same filename. will this be a problem?
    return _data_directory + "/"
    + _block_filename + "_"
                           + std::to_string(_current_block_file_number) + "."
                           + _file_extension;

}

std::string ChainWriter::read_block(const FileInfo &block_location) {
  FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

pFile = fopen( block_location.file_name.c_str() , "r" );
lSize = block_location.end - block_location.start;
fseek (pFile , block_location.start, SEEK_SET);
buffer = (char*) malloc (sizeof(char)*lSize);
if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

result = fread (buffer,1,lSize,pFile);
if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
fclose (pFile);
//free (buffer);
std::string file(buffer, lSize);
return file;

}

std::string ChainWriter::read_undo_block(const FileInfo &undo_block_location) {
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen( undo_block_location.file_name.c_str() , "r" );

    lSize = undo_block_location.end - undo_block_location.start;
    fseek (pFile , undo_block_location.start, SEEK_SET);
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    fclose (pFile);
    //free (buffer);
    std::string file(buffer, lSize);
    return file;

}





