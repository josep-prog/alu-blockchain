#include "cli.h"

/**
 * cmd_info - Displays information about the Blockchain
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments (unused)
 * @argv:  Arguments (unused)
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_info(state_t *state, int argc, char **argv)
{
	blockchain_t *blockchain = state->blockchain;
	block_t *last = llist_get_tail(blockchain->chain);
	uint8_t pub[EC_PUB_LEN];

	(void)argc;
	(void)argv;
	printf("Blocks:                      %d\n",
		llist_size(blockchain->chain));
	printf("Unspent transaction outputs: %d\n",
		llist_size(blockchain->unspent));
	printf("Pending transactions (pool): %d\n",
		llist_size(state->tx_pool));
	printf("Next Block difficulty:       %u\n",
		blockchain_difficulty(blockchain));
	if (last)
	{
		printf("Latest Block hash:           ");
		print_hex(last->hash, SHA256_DIGEST_LENGTH);
		printf("\n");
	}
	if (ec_to_pub(state->wallet, pub))
	{
		printf("Wallet balance:              %lu\n",
			(unsigned long)unspent_balance(blockchain->unspent,
				pub));
		printf("Wallet address:              ");
		print_hex(pub, EC_PUB_LEN);
		printf("\n");
	}

	return (0);
}
