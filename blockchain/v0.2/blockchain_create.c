#include "blockchain.h"

/**
 * blockchain_create - Creates a Blockchain containing the Genesis Block
 *
 * Return: Pointer to the created Blockchain, or NULL upon failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis;

	blockchain = calloc(1, sizeof(*blockchain));
	genesis = calloc(1, sizeof(*genesis));
	if (!blockchain || !genesis)
	{
		free(blockchain);
		free(genesis);
		return (NULL);
	}

	genesis->info.timestamp = GENESIS_TIMESTAMP;
	memcpy(genesis->data.buffer, GENESIS_DATA, GENESIS_DATA_LEN);
	genesis->data.len = GENESIS_DATA_LEN;
	memcpy(genesis->hash, GENESIS_HASH, SHA256_DIGEST_LENGTH);

	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain ||
	    llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) == -1)
	{
		llist_destroy(blockchain->chain, 0, NULL);
		free(genesis);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);
}
