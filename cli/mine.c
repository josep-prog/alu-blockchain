#include "cli.h"

/**
 * pool_process - Moves the valid transactions of the pool into a Block
 *
 * @state: State of the CLI session
 * @block: Pointer to the Block being built
 *
 * Description: Transactions are processed one by one. Each one is verified
 * against the unspent outputs left by the transactions already included in
 * the Block. Invalid transactions are deleted. The pool is emptied.
 */
static void pool_process(state_t *state, block_t *block)
{
	transaction_t *tx;
	llist_t *available;
	int valid;

	while (llist_size(state->tx_pool) > 0)
	{
		tx = llist_pop(state->tx_pool);
		if (!tx)
			break;

		available = unspent_available(state->blockchain->unspent,
			block->transactions);
		valid = available && transaction_is_valid(tx, available) &&
			llist_add_node(block->transactions, tx,
				ADD_NODE_REAR) == 0;
		if (available)
			llist_destroy(available, 0, NULL);

		printf("Transaction ");
		print_hex(tx->id, SHA256_DIGEST_LENGTH);
		printf(valid ? ": included\n" : ": invalid, deleted\n");
		if (!valid)
			transaction_destroy(tx);
	}
}

/**
 * block_build - Creates the next Block of the Blockchain and mines it
 *
 * @state: State of the CLI session
 * @prev:  Pointer to the latest Block of the Blockchain
 *
 * Return: Pointer to the mined Block, or NULL upon failure
 */
static block_t *block_build(state_t *state, block_t *prev)
{
	block_t *block;
	transaction_t *coinbase;

	block = block_create(prev, (int8_t *)"hblk-cli", 8);
	if (!block)
		return (NULL);

	block->info.difficulty = blockchain_difficulty(state->blockchain);

	coinbase = coinbase_create(state->wallet, block->info.index);
	if (!coinbase ||
	    llist_add_node(block->transactions, coinbase, ADD_NODE_FRONT) == -1)
	{
		transaction_destroy(coinbase);
		block_destroy(block);
		return (NULL);
	}

	pool_process(state, block);
	block_mine(block);

	return (block);
}

/**
 * cmd_mine - Mines a Block and adds it to the Blockchain
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments (unused)
 * @argv:  Arguments (unused)
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_mine(state_t *state, int argc, char **argv)
{
	blockchain_t *blockchain = state->blockchain;
	block_t *block, *prev;
	llist_t *unspent;

	(void)argc;
	(void)argv;
	prev = llist_get_tail(blockchain->chain);
	block = prev ? block_build(state, prev) : NULL;
	if (!block)
	{
		printf("Failed to create Block\n");
		return (1);
	}

	if (block_is_valid(block, prev, blockchain->unspent) != 0)
	{
		printf("Mined Block is invalid, discarded\n");
		block_destroy(block);
		return (1);
	}
	unspent = update_unspent(block->transactions, block->hash,
		blockchain->unspent);
	if (!unspent || llist_add_node(blockchain->chain, block,
		ADD_NODE_REAR) == -1)
	{
		if (unspent)
			blockchain->unspent = unspent;
		printf("Failed to add Block to the Blockchain\n");
		block_destroy(block);
		return (1);
	}
	blockchain->unspent = unspent;

	printf("Block mined: index %u, difficulty %u, %d transaction(s)\nHash: ",
		block->info.index, block->info.difficulty,
		llist_size(block->transactions));
	print_hex(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");

	return (0);
}
