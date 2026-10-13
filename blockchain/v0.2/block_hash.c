#include "blockchain.h"

/**
 * block_hash - Computes the hash of a Block
 *
 * @block:    Pointer to the Block to be hashed
 * @hash_buf: Buffer in which the resulting hash is stored
 *
 * Description: The hash covers the Block's info, followed by the
 * data.len bytes of its data buffer. block->hash is left unchanged.
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t len;

	if (!block || !hash_buf || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (NULL);

	len = sizeof(block->info) + block->data.len;

	return (SHA256((unsigned char const *)block, len, hash_buf));
}
