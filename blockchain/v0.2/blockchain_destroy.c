#include "blockchain.h"

/**
 * blockchain_destroy - Deletes a Blockchain, along with all its Blocks
 *
 * @blockchain: Pointer to the Blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;

	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	free(blockchain);
}
