#include <ctype.h>
#include <errno.h>

#include "cli.h"

/**
 * address_parse - Converts a hexadecimal address into an EC public key
 *
 * @hex: Address, as 130 hexadecimal characters
 * @pub: Buffer in which the public key is stored
 *
 * Return: 1 upon success, 0 if the address is malformed
 */
static int address_parse(char const *hex, uint8_t pub[EC_PUB_LEN])
{
	char byte[3] = {0, 0, 0};
	size_t i;

	if (strlen(hex) != PUB_HEX_LEN)
		return (0);

	for (i = 0; i < PUB_HEX_LEN; i++)
	{
		if (!isxdigit((unsigned char)hex[i]))
			return (0);
	}
	for (i = 0; i < EC_PUB_LEN; i++)
	{
		byte[0] = hex[i * 2];
		byte[1] = hex[i * 2 + 1];
		pub[i] = (uint8_t)strtoul(byte, NULL, 16);
	}

	return (1);
}

/**
 * amount_parse - Converts a string into an amount of coins
 *
 * @str:    String to convert
 * @amount: Set to the converted amount
 *
 * Return: 1 upon success, 0 if the amount is not a positive integer
 */
static int amount_parse(char const *str, uint32_t *amount)
{
	unsigned long value;
	char *end;

	if (!isdigit((unsigned char)*str))
		return (0);

	errno = 0;
	value = strtoul(str, &end, 10);
	if (errno || *end || value == 0 || value > 0xFFFFFFFFul)
		return (0);

	*amount = (uint32_t)value;
	return (1);
}

/**
 * tx_send - Creates a transaction and adds it to the transaction pool
 *
 * @state:    State of the CLI session
 * @receiver: Public key of the receiver
 * @amount:   Amount of coins to send
 *
 * Description: The unspent outputs already used by pending transactions are
 * not available. The list of unspent outputs is not updated.
 *
 * Return: 0 upon success, 1 upon failure
 */
static int tx_send(state_t *state, EC_KEY *receiver, uint32_t amount)
{
	llist_t *available;
	transaction_t *tx = NULL;
	uint8_t pub[EC_PUB_LEN];
	int status = 1;

	available = unspent_available(state->blockchain->unspent,
		state->tx_pool);
	if (available && ec_to_pub(state->wallet, pub))
		tx = transaction_create(state->wallet, receiver, amount,
			available);
	if (!tx)
	{
		printf("Failed to create transaction: not enough coins ");
		printf("(available: %lu, total balance: %lu)\n", available ?
			(unsigned long)unspent_balance(available, pub) : 0,
			(unsigned long)unspent_balance(
				state->blockchain->unspent, pub));
	}
	else if (!transaction_is_valid(tx, available) ||
		 llist_add_node(state->tx_pool, tx, ADD_NODE_REAR) == -1)
	{
		printf("Transaction invalid, not added to the pool\n");
		transaction_destroy(tx);
	}
	else
	{
		printf("Transaction created, signed and added to the pool\nID: ");
		print_hex(tx->id, SHA256_DIGEST_LENGTH);
		printf("\nPending transactions: %d\n",
			llist_size(state->tx_pool));
		status = 0;
	}
	if (available)
		llist_destroy(available, 0, NULL);

	return (status);
}

/**
 * cmd_send - Sends coins to an address
 *
 * @state: State of the CLI session
 * @argc:  Number of arguments
 * @argv:  Arguments: send <amount> <address>
 *
 * Return: 0 upon success, 1 upon failure
 */
int cmd_send(state_t *state, int argc, char **argv)
{
	uint8_t pub[EC_PUB_LEN];
	EC_KEY *receiver;
	uint32_t amount;
	int status;

	if (argc != 3)
	{
		printf("Usage: send <amount> <address>\n");
		return (1);
	}
	if (!amount_parse(argv[1], &amount))
	{
		printf("Invalid amount: %s\n", argv[1]);
		return (1);
	}
	receiver = address_parse(argv[2], pub) ? ec_from_pub(pub) : NULL;
	if (!receiver)
	{
		printf("Invalid address: expected an EC public key ");
		printf("(%d hexadecimal characters)\n", PUB_HEX_LEN);
		return (1);
	}

	status = tx_send(state, receiver, amount);
	EC_KEY_free(receiver);

	return (status);
}
