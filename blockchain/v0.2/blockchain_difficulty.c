#include "blockchain.h"

/**
 * blockchain_difficulty - Computes the difficulty to assign to the next
 * Block of a Blockchain
 *
 * @blockchain: Pointer to the Blockchain to analyze
 *
 * Return: Difficulty to be assigned to the next Block in the Blockchain
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	block_t *last, *adjusted;
	uint64_t expected, actual;

	if (!blockchain || !blockchain->chain)
		return (0);

	last = llist_get_tail(blockchain->chain);
	if (!last)
		return (0);
	if (last->info.index == 0 ||
	    last->info.index % DIFFICULTY_ADJUSTMENT_INTERVAL != 0)
		return (last->info.difficulty);

	adjusted = llist_get_node_at(blockchain->chain,
		last->info.index + 1 - DIFFICULTY_ADJUSTMENT_INTERVAL);
	if (!adjusted)
		return (last->info.difficulty);

	expected = DIFFICULTY_ADJUSTMENT_INTERVAL * BLOCK_GENERATION_INTERVAL;
	actual = last->info.timestamp - adjusted->info.timestamp;

	if (actual * 2 < expected)
		return (last->info.difficulty + 1);
	if (actual > expected * 2)
		return (last->info.difficulty ? last->info.difficulty - 1 : 0);

	return (last->info.difficulty);
}
