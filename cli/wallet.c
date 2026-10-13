#include "cli.h"

/**
 * cmd_wallet_load - Loads a wallet (EC key pair) from a folder
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments
 * @argv:  Arguments: wallet_load <path>
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_wallet_load(state_t *state, int argc, char **argv)
{
	EC_KEY *wallet;
	uint8_t pub[EC_PUB_LEN];

	if (argc != 2)
	{
		printf("Usage: wallet_load <path>\n");
		return (1);
	}

	wallet = ec_load(argv[1]);
	if (!wallet || !ec_to_pub(wallet, pub))
	{
		EC_KEY_free(wallet);
		printf("Failed to load wallet from %s\n", argv[1]);
		return (1);
	}

	EC_KEY_free(state->wallet);
	state->wallet = wallet;
	printf("Wallet loaded from %s\nAddress: ", argv[1]);
	print_hex(pub, EC_PUB_LEN);
	printf("\n");

	return (0);
}

/**
 * cmd_wallet_save - Saves the wallet (EC key pair) in a folder, in PEM format
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments
 * @argv:  Arguments: wallet_save <path>
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_wallet_save(state_t *state, int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: wallet_save <path>\n");
		return (1);
	}

	if (!ec_save(state->wallet, argv[1]))
	{
		printf("Failed to save wallet in %s\n", argv[1]);
		return (1);
	}

	printf("Wallet saved in %s (%s, %s)\n", argv[1], PRI_FILENAME,
		PUB_FILENAME);

	return (0);
}
