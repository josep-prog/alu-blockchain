#include "blockchain.h"

/**
 * block_hash - Computes the hash of a Block
 *
 * @block:    Pointer to the Block to be hashed
 * @hash_buf: Buffer in which the resulting hash is stored
 *
 * Description: The hash covers the Block's info, the data.len bytes of its
 * data buffer, and the ID of each of its transactions.
 * block->hash is left unchanged.
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	transaction_t *transaction;
	uint8_t *buf;
	size_t len;
	int i, nb_tx;

	if (!block || !hash_buf || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (NULL);

	nb_tx = block->transactions ? llist_size(block->transactions) : 0;
	if (nb_tx < 0)
		nb_tx = 0;

	len = sizeof(block->info) + block->data.len;
	buf = malloc(len + nb_tx * SHA256_DIGEST_LENGTH);
	if (!buf)
		return (NULL);

	memcpy(buf, block, len);
	for (i = 0; i < nb_tx; i++)
	{
		transaction = llist_get_node_at(block->transactions, i);
		if (!transaction)
		{
			free(buf);
			return (NULL);
		}
		memcpy(buf + len, transaction->id, SHA256_DIGEST_LENGTH);
		len += SHA256_DIGEST_LENGTH;
	}

	SHA256(buf, len, hash_buf);
	free(buf);

	return (hash_buf);
}
