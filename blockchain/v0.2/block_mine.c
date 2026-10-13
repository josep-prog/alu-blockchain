#include "blockchain.h"

/**
 * block_mine - Mines a Block in order to insert it in the Blockchain
 *
 * @block: Pointer to the Block to be mined
 *
 * Description: Increments the nonce until the hash of the Block matches
 * its difficulty. The resulting hash is stored in the Block.
 */
void block_mine(block_t *block)
{
	if (!block)
		return;

	block->info.nonce = 0;
	while (block_hash(block, block->hash) &&
	       !hash_matches_difficulty(block->hash, block->info.difficulty))
		block->info.nonce++;
}
