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

std::unique_ptr<BlockRecord> ChainWriter :: store_block(const Block& block,
                                                        const UndoBlock& undo_block,
                                                        uint32_t height){


    std::unique_ptr<FileInfo> block_info = ChainWriter::write_block(Block::serialize(block));
    std::unique_ptr<FileInfo> undo_info = ChainWriter::write_block(UndoBlock::serialize(undo_block));

    uint32_t trx_size = block.transactions.size();

//    std::unique_ptr<BlockRecord> record = std::make_unique<BlockRecord>(std::move(block.block_header),
//                                                                        trx_size,
//                                                                        height,
//                                                                        *info_list[0],
//                                                                        *info_list[1]);

    //make vector <block, undo block>
    //call write block, get File Info
    //create and return block Record

   // return record;
}



std::unique_ptr<FileInfo> ChainWriter::write_block( std::string block) {
    FILE *pFile;

        int n = block.length();
        char char_block[n + 1];
        strcpy(char_block, block.c_str());


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

    //ToDo: check for file size, and open new file accordingly
    return info;
}


std::unique_ptr<FileInfo> ChainWriter::write_undo_block( std::string block) {
    FILE *pFile;

    int n = block.length();
    char char_block[n + 1];
    strcpy(char_block, block.c_str());


    //ToDo: can we assume block and undo block have same max block size
    if (_current_undo_offset + n > get_max_undo_file_size()) { //check for current size of file{
        _current_undo_offset = 0;
        _current_undo_file_number = +1;
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

    //ToDo: check for file size, and open new file accordingly
    return info;
}



std::string ChainWriter::get_filename(bool is_undo) {
    if (is_undo) {
        std::string filename = _data_directory + "/"
                                + _undo_filename + "_"
                                + std::to_string(_current_undo_file_number)
                                + _file_extension;
        return filename;
    }
    std::string filename = _data_directory + "/"
                           + _block_filename + "_"
                           + std::to_string(_current_block_file_number)
                           + _file_extension;
    return filename;
}

std::string read_block(const FileInfo& block_location){
//fopen, fseek, fread, and fclose
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen( block_location.file_name.c_str() , "r" ); //rb?
    //if (pFile==NULL) {fputs ("File error",stderr); exit (1);} //not too necessary

// obtain file size:
    fseek (pFile , block_location.end - block_location.start , block_location.start);
    //SEEK_SET
    lSize = ftell (pFile);
    rewind (pFile);

// allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

// copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

/* the whole file is now loaded in the memory buffer. */

// terminate
    fclose (pFile);
    free (buffer);

    std::string file(buffer, lSize);
    return file;
}



std::string read_undo_block(const FileInfo& block_location){
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen( block_location.file_name.c_str() , "r" ); //rb?
    //if (pFile==NULL) {fputs ("File error",stderr); exit (1);} //not too necessary

// obtain file size:
    fseek (pFile , block_location.end - block_location.start , block_location.start);
    //SEEK_SET
    lSize = ftell (pFile);
    rewind (pFile);

// allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

// copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

/* the whole file is now loaded in the memory buffer. */

// terminate
    fclose (pFile);
    free (buffer);

    std::string file(buffer, lSize);
    return file;
}
