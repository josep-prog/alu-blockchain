#include "blockchain.h"

/**
 * genesis_is_valid - Checks that a Block matches the Genesis Block
 *
 * @block: Pointer to the Block to check
 *
 * Return: 0 if @block matches the Genesis Block, 1 otherwise
 */
static int genesis_is_valid(block_t const *block)
{
	block_t genesis;

	memset(&genesis, 0, sizeof(genesis));
	genesis.info.timestamp = GENESIS_TIMESTAMP;
	memcpy(genesis.data.buffer, GENESIS_DATA, GENESIS_DATA_LEN);
	genesis.data.len = GENESIS_DATA_LEN;
	memcpy(genesis.hash, GENESIS_HASH, SHA256_DIGEST_LENGTH);

	return (memcmp(block, &genesis, sizeof(genesis)) != 0);
}

/**
 * block_is_valid - Verifies that a Block is valid
 *
 * @block:      Pointer to the Block to check
 * @prev_block: Pointer to the previous Block in the Blockchain, or NULL
 *              if @block is the first Block of the chain
 *
 * Return: 0 if the Block is valid, 1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!block)
		return (1);
	if (block->info.index == 0)
		return (prev_block ? 1 : genesis_is_valid(block));
	if (!prev_block)
		return (1);

	if (block->info.index != prev_block->info.index + 1)
		return (1);
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
	    prev_block->data.len > BLOCKCHAIN_DATA_MAX)
		return (1);

	if (!block_hash(prev_block, hash) ||
	    memcmp(hash, prev_block->hash, SHA256_DIGEST_LENGTH) != 0 ||
	    memcmp(hash, block->info.prev_hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	if (!block_hash(block, hash) ||
	    memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);
	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (1);

	return (0);
}
