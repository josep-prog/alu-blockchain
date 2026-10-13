#include "transaction.h"

/**
 * transaction_hash - Computes the ID (hash) of a transaction
 *
 * @transaction: Pointer to the transaction to compute the hash of
 * @hash_buf:    Buffer in which to store the computed hash
 *
 * Description: The hashed buffer holds each input's block_hash, tx_id and
 * tx_out_hash, followed by each output's hash.
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *buf, *cursor;
	tx_in_t *in;
	tx_out_t *out;
	int i, nb_in, nb_out;

	if (!transaction || !hash_buf)
		return (NULL);

	nb_in = llist_size(transaction->inputs);
	nb_out = llist_size(transaction->outputs);
	if (nb_in < 0 || nb_out < 0)
		return (NULL);

	buf = malloc(nb_in * TX_IN_REF_LEN + nb_out * SHA256_DIGEST_LENGTH + 1);
	if (!buf)
		return (NULL);

	cursor = buf;
	for (i = 0; i < nb_in; i++, cursor += TX_IN_REF_LEN)
	{
		in = llist_get_node_at(transaction->inputs, i);
		memcpy(cursor, in, TX_IN_REF_LEN);
	}
	for (i = 0; i < nb_out; i++, cursor += SHA256_DIGEST_LENGTH)
	{
		out = llist_get_node_at(transaction->outputs, i);
		memcpy(cursor, out->hash, SHA256_DIGEST_LENGTH);
	}

	SHA256(buf, cursor - buf, hash_buf);
	free(buf);

	return (hash_buf);
}
