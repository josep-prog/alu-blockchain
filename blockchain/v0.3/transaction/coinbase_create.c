#include "transaction.h"

/**
 * coinbase_create - Creates a coinbase transaction
 *
 * @receiver:    Public key of the miner, who will receive the coinbase coins
 * @block_index: Index of the Block the coinbase transaction will belong to
 *
 * Description: The single input is zeroed and not signed. To make it unique,
 * the Block index is copied in the first 4 bytes of its tx_out_hash.
 *
 * Return: Pointer to the created transaction, or NULL upon failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	uint8_t pub[EC_PUB_LEN];
	transaction_t *coinbase;
	tx_in_t *in;
	tx_out_t *out;

	if (!receiver || !ec_to_pub(receiver, pub))
		return (NULL);

	coinbase = calloc(1, sizeof(*coinbase));
	in = calloc(1, sizeof(*in));
	out = tx_out_create(COINBASE_AMOUNT, pub);
	if (coinbase)
	{
		coinbase->inputs = llist_create(MT_SUPPORT_FALSE);
		coinbase->outputs = llist_create(MT_SUPPORT_FALSE);
	}
	if (!coinbase || !in || !out || !coinbase->inputs || !coinbase->outputs)
		goto fail;

	memcpy(in->tx_out_hash, &block_index, sizeof(block_index));
	if (llist_add_node(coinbase->inputs, in, ADD_NODE_REAR) == -1 ||
	    llist_add_node(coinbase->outputs, out, ADD_NODE_REAR) == -1 ||
	    !transaction_hash(coinbase, coinbase->id))
		goto fail;

	return (coinbase);

fail:
	if (coinbase && coinbase->inputs)
		llist_destroy(coinbase->inputs, 0, NULL);
	if (coinbase && coinbase->outputs)
		llist_destroy(coinbase->outputs, 0, NULL);
	free(coinbase);
	free(in);
	free(out);
	return (NULL);
}
