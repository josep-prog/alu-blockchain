#include "transaction.h"

/**
 * is_zeroed - Checks that a buffer only contains null bytes
 *
 * @buf: Pointer to the buffer to check
 * @len: Number of bytes to check
 *
 * Return: 1 if the buffer is zeroed, 0 otherwise
 */
static int is_zeroed(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
	{
		if (buf[i])
			return (0);
	}

	return (1);
}

/**
 * coinbase_is_valid - Checks whether a coinbase transaction is valid
 *
 * @coinbase:    Pointer to the coinbase transaction to verify
 * @block_index: Index of the Block the coinbase transaction will belong to
 *
 * Return: 1 if the coinbase transaction is valid, 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	tx_in_t *in;
	tx_out_t *out;

	if (!coinbase)
		return (0);
	if (!transaction_hash(coinbase, hash) ||
	    memcmp(hash, coinbase->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	if (llist_size(coinbase->inputs) != 1 ||
	    llist_size(coinbase->outputs) != 1)
		return (0);

	in = llist_get_node_at(coinbase->inputs, 0);
	out = llist_get_node_at(coinbase->outputs, 0);
	if (!in || !out)
		return (0);

	if (memcmp(in->tx_out_hash, &block_index, sizeof(block_index)) != 0)
		return (0);
	if (!is_zeroed(in->block_hash, sizeof(in->block_hash)) ||
	    !is_zeroed(in->tx_id, sizeof(in->tx_id)) ||
	    !is_zeroed(in->sig.sig, sizeof(in->sig.sig)) || in->sig.len != 0)
		return (0);

	return (out->amount == COINBASE_AMOUNT);
}
