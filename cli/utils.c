#include "cli.h"

/**
 * is_referenced - Checks if an unspent output is used by pending transactions
 *
 * @unspent:      Pointer to the unspent transaction output to check
 * @transactions: List of transactions
 *
 * Return: 1 if an input of @transactions refers to @unspent, 0 otherwise
 */
static int is_referenced(unspent_tx_out_t const *unspent,
	llist_t *transactions)
{
	transaction_t *transaction;
	tx_in_t *in;
	int i, j, nb_tx, nb_in;

	nb_tx = llist_size(transactions);
	for (i = 0; i < nb_tx; i++)
	{
		transaction = llist_get_node_at(transactions, i);
		nb_in = transaction ? llist_size(transaction->inputs) : 0;
		for (j = 0; j < nb_in; j++)
		{
			in = llist_get_node_at(transaction->inputs, j);
			if (in &&
			    !memcmp(unspent->block_hash, in->block_hash,
				SHA256_DIGEST_LENGTH) &&
			    !memcmp(unspent->tx_id, in->tx_id,
				SHA256_DIGEST_LENGTH) &&
			    !memcmp(unspent->out.hash, in->tx_out_hash,
				SHA256_DIGEST_LENGTH))
				return (1);
		}
	}

	return (0);
}

/**
 * unspent_available - Lists the unspent outputs that are not already used
 * by a list of pending transactions
 *
 * @all_unspent:  List of all unspent transaction outputs
 * @transactions: List of pending transactions
 *
 * Description: The returned list shares its nodes with @all_unspent. It must
 * be destroyed without destroying its nodes.
 *
 * Return: Pointer to the list of available unspent outputs, NULL on failure
 */
llist_t *unspent_available(llist_t *all_unspent, llist_t *transactions)
{
	llist_t *available;
	unspent_tx_out_t *unspent;
	int i, size;

	available = llist_create(MT_SUPPORT_FALSE);
	if (!available)
		return (NULL);

	size = llist_size(all_unspent);
	for (i = 0; i < size; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent || is_referenced(unspent, transactions))
			continue;
		if (llist_add_node(available, unspent, ADD_NODE_REAR) == -1)
		{
			llist_destroy(available, 0, NULL);
			return (NULL);
		}
	}

	return (available);
}

/**
 * unspent_balance - Computes the amount of coins owned by a public key
 *
 * @unspent: List of unspent transaction outputs
 * @pub:     Public key of the owner
 *
 * Return: Total amount of the unspent outputs belonging to @pub
 */
uint64_t unspent_balance(llist_t *unspent, uint8_t const pub[EC_PUB_LEN])
{
	unspent_tx_out_t *node;
	uint64_t balance = 0;
	int i, size = llist_size(unspent);

	for (i = 0; i < size; i++)
	{
		node = llist_get_node_at(unspent, i);
		if (node && memcmp(node->out.pub, pub, EC_PUB_LEN) == 0)
			balance += node->out.amount;
	}

	return (balance);
}

/**
 * print_hex - Prints a buffer in its hexadecimal form
 *
 * @buf: Pointer to the buffer to be printed
 * @len: Number of bytes from @buf to be printed
 */
void print_hex(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", buf[i]);
}
