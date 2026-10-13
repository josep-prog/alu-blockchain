#include "blockchain.h"

/**
 * blockchain_destroy - Deletes a Blockchain, along with all its Blocks
 * and its list of unspent transaction outputs
 *
 * @blockchain: Pointer to the Blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;

	if (blockchain->chain)
		llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 1, free);
	free(blockchain);
}
