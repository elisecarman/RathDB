#include <chain.h>
#include <rathcrypto.h>


std::unique_ptr<UndoBlock> Chain:: make_undo_block(std::unique_ptr<Block> block) {
//    //get transactions
//    std::vector<uint32_t> transaction_hashes;
//    std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;
//    std::vector<std::unique_ptr<Transaction>> block_trx = block->get_transactions();
//
//    for (int a = 0; a < block_trx.size(); a = a + 1) {
//        Transaction trx = *block_trx[a];
//        transaction_hashes.insert(transaction_hashes.end(), 1,
//                                  RathCrypto::hash(Transaction::serialize(trx)));
//
//        std::vector<uint32_t> utxo;
//        std::vector<uint32_t> amounts;
//        std::vector<uint32_t> public_keys;
//
//        for (int b = 0; b < trx.transaction_inputs.size(); b = b + 1) {
//            std::tuple<uint32_t ,uint32_t > tup = _coin_database->return_matching_utxo(
//                    std::move(trx.transaction_inputs[a]));
//            utxo.insert(utxo.end(), 1, trx.transaction_inputs[a]->utxo_index);
//            amounts.insert(amounts.end(), 1, std::get<0>(tup));
//            public_keys.insert(public_keys.end(), 1,  std::get<1>(tup));
//        }
//
//        std:: unique_ptr<UndoCoinRecord> record = std::make_unique<UndoCoinRecord>(
//                trx.version, utxo, amounts, public_keys);
//
//        undo_coin_records.insert(undo_coin_records.end(), std::move(record));
//        //get transaction inputs
//        //-> fill the coin record vector
//        //-> fill the transaction hash vector
//    }
//    std::unique_ptr<UndoBlock> u_block = std::make_unique<UndoBlock>(transaction_hashes, std::move(undo_coin_records));
//
//    return u_block;
}