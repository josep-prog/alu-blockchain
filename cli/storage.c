#include "cli.h"

/**
 * cmd_load - Loads a Blockchain from a file, overriding the local one
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments
 * @argv:  Arguments: load <path>
 *
 * Description: Upon failure, the local Blockchain is left untouched. Upon
 * success, the pending transactions are dropped, since they refer to the
 * previous Blockchain.
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_load(state_t *state, int argc, char **argv)
{
	blockchain_t *blockchain;
	transaction_t *tx;

	if (argc != 2)
	{
		printf("Usage: load <path>\n");
		return (1);
	}

	blockchain = blockchain_deserialize(argv[1]);
	if (!blockchain)
	{
		printf("Failed to load Blockchain from %s: ", argv[1]);
		printf("file not found or invalid format\n");
		return (1);
	}

	blockchain_destroy(state->blockchain);
	state->blockchain = blockchain;
	while (llist_size(state->tx_pool) > 0)
	{
		tx = llist_pop(state->tx_pool);
		transaction_destroy(tx);
	}

	printf("Blockchain loaded from %s: %d Block(s), %d unspent output(s)\n",
		argv[1], llist_size(blockchain->chain),
		llist_size(blockchain->unspent));

	return (0);
}

/**
 * cmd_save - Saves the local Blockchain into a file
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments
 * @argv:  Arguments: save <path>
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_save(state_t *state, int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: save <path>\n");
		return (1);
	}

	if (blockchain_serialize(state->blockchain, argv[1]) == -1)
	{
		printf("Failed to save Blockchain in %s\n", argv[1]);
		return (1);
	}

	printf("Blockchain saved in %s: %d Block(s)\n", argv[1],
		llist_size(state->blockchain->chain));

	return (0);
}
