#include <block_info_database.h>
#include <db.h>
#include <block_record.h>

BlockInfoDatabase::BlockInfoDatabase() : _database(std::make_unique<Database>()) {

}

void BlockInfoDatabase::store_block_record(uint32_t hash, const BlockRecord& record) {
    //might need to check for null values?
    std::string serial = BlockRecord::serialize(record);
    _database->put_safely(std::to_string(hash), std::move(serial));

}

std::unique_ptr<BlockRecord> BlockInfoDatabase::get_block_record(uint32_t block_hash) {

    std::string serial = _database->get_safely(std::to_string(block_hash));
    return BlockRecord::deserialize(serial);
}



