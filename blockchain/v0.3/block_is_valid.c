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

	return (memcmp(&block->info, &genesis.info, sizeof(genesis.info)) ||
		memcmp(&block->data, &genesis.data, sizeof(genesis.data)) ||
		memcmp(block->hash, genesis.hash, sizeof(genesis.hash)));
}

/**
 * transactions_are_valid - Checks the list of transactions of a Block
 *
 * @block:       Pointer to the Block to check
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Description: A Block must contain at least one transaction. The first
 * one must be a valid coinbase transaction, the others must be valid
 * regular transactions.
 *
 * Return: 0 if the transactions are valid, 1 otherwise
 */
static int transactions_are_valid(block_t const *block, llist_t *all_unspent)
{
	transaction_t *transaction;
	int i, size;

	if (!block->transactions)
		return (1);
	size = llist_size(block->transactions);
	if (size < 1)
		return (1);

	transaction = llist_get_node_at(block->transactions, 0);
	if (!coinbase_is_valid(transaction, block->info.index))
		return (1);

	for (i = 1; i < size; i++)
	{
		transaction = llist_get_node_at(block->transactions, i);
		if (!transaction_is_valid(transaction, all_unspent))
			return (1);
	}

	return (0);
}

/**
 * block_is_valid - Verifies that a Block is valid
 *
 * @block:       Pointer to the Block to check
 * @prev_block:  Pointer to the previous Block in the Blockchain, or NULL
 *               if @block is the first Block of the chain
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 0 if the Block is valid, 1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
	llist_t *all_unspent)
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

	return (transactions_are_valid(block, all_unspent));
}
