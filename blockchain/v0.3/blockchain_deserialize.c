#include <stdio.h>

#include "blockchain.h"

#define ITEM_IN 0
#define ITEM_OUT 1
#define ITEM_UNSPENT 2

/**
 * read_bytes - Reads bytes from a file, swapping them if needed
 *
 * @file: Stream to read from
 * @ptr:  Buffer in which the bytes are stored
 * @size: Number of bytes to read
 * @swap: Non-zero to reverse the bytes read (endianness mismatch)
 *
 * Return: 1 upon success, 0 upon failure
 */
static int read_bytes(FILE *file, void *ptr, size_t size, int swap)
{
	uint8_t *bytes = ptr, tmp;
	size_t i;

	if (fread(ptr, 1, size, file) != size)
		return (0);

	for (i = 0; swap && i < size / 2; i++)
	{
		tmp = bytes[i];
		bytes[i] = bytes[size - 1 - i];
		bytes[size - 1 - i] = tmp;
	}

	return (1);
}

/**
 * items_deserialize - Reads a list of transaction inputs, transaction
 * outputs, or unspent transaction outputs from a file
 *
 * @file:  Stream to read from
 * @count: Number of items to read
 * @type:  ITEM_IN, ITEM_OUT or ITEM_UNSPENT
 * @swap:  Non-zero if the endianness of the file differs from the host's
 *
 * Return: Pointer to the list of items read, or NULL upon failure
 */
static llist_t *items_deserialize(FILE *file, uint32_t count, int type,
	int swap)
{
	llist_t *list = llist_create(MT_SUPPORT_FALSE);
	tx_in_t *in;
	unspent_tx_out_t *unspent;
	tx_out_t *out;
	uint8_t sig_len = 0;
	void *item = NULL;
	int ok = list != NULL;

	for (; ok && count; count--)
	{
		item = calloc(1, sizeof(*unspent) + sizeof(*in));
		in = item, unspent = item, ok = item != NULL;
		if (ok && type == ITEM_IN)
		{
			ok = read_bytes(file, in, TX_IN_REF_LEN, 0) &&
				read_bytes(file, in->sig.sig, SIG_MAX_LEN, 0) &&
				read_bytes(file, &sig_len, 1, 0) &&
				sig_len <= SIG_MAX_LEN;
			in->sig.len = sig_len;
		}
		else if (ok)
		{
			out = type == ITEM_OUT ? item : &unspent->out;
			ok = (type == ITEM_OUT || read_bytes(file, unspent,
					SHA256_DIGEST_LENGTH * 2, 0)) &&
				read_bytes(file, &out->amount, 4, swap) &&
				read_bytes(file, out->pub, EC_PUB_LEN, 0) &&
				read_bytes(file, out->hash,
					SHA256_DIGEST_LENGTH, 0);
		}
		ok = ok && llist_add_node(list, item, ADD_NODE_REAR) == 0;
	}
	if (ok)
		return (list);
	free(item);
	if (list)
		llist_destroy(list, 1, free);
	return (NULL);
}

/**
 * tx_deserialize - Reads a single transaction from a file
 *
 * @file: Stream to read from
 * @swap: Non-zero if the endianness of the file differs from the host's
 *
 * Return: Pointer to the allocated transaction, or NULL upon failure
 */
static transaction_t *tx_deserialize(FILE *file, int swap)
{
	transaction_t *transaction;
	uint32_t nb_in, nb_out;

	transaction = calloc(1, sizeof(*transaction));
	if (!transaction)
		return (NULL);

	if (read_bytes(file, transaction->id, SHA256_DIGEST_LENGTH, 0) &&
	    read_bytes(file, &nb_in, 4, swap) &&
	    read_bytes(file, &nb_out, 4, swap))
	{
		transaction->inputs = items_deserialize(file, nb_in, ITEM_IN,
			swap);
		if (transaction->inputs)
			transaction->outputs = items_deserialize(file, nb_out,
				ITEM_OUT, swap);
	}
	if (!transaction->inputs || !transaction->outputs)
	{
		transaction_destroy(transaction);
		return (NULL);
	}

	return (transaction);
}

/**
 * block_deserialize - Reads a single Block from a file
 *
 * @file: Stream to read from
 * @swap: Non-zero if the endianness of the file differs from the host's
 *
 * Return: Pointer to the allocated Block, or NULL upon failure
 */
static block_t *block_deserialize(FILE *file, int swap)
{
	block_t *block = calloc(1, sizeof(*block));
	transaction_t *transaction;
	int32_t nb_tx = 0;
	int ok;

	if (!block)
		return (NULL);
	ok = read_bytes(file, &block->info.index, 4, swap) &&
		read_bytes(file, &block->info.difficulty, 4, swap) &&
		read_bytes(file, &block->info.timestamp, 8, swap) &&
		read_bytes(file, &block->info.nonce, 8, swap) &&
		read_bytes(file, block->info.prev_hash, SHA256_DIGEST_LENGTH, 0) &&
		read_bytes(file, &block->data.len, 4, swap) &&
		block->data.len <= BLOCKCHAIN_DATA_MAX &&
		read_bytes(file, block->data.buffer, block->data.len, 0) &&
		read_bytes(file, block->hash, SHA256_DIGEST_LENGTH, 0) &&
		read_bytes(file, &nb_tx, 4, swap) && nb_tx >= -1;
	if (ok && nb_tx >= 0)
	{
		block->transactions = llist_create(MT_SUPPORT_FALSE);
		ok = block->transactions != NULL;
	}
	for (; ok && nb_tx > 0; nb_tx--)
	{
		transaction = tx_deserialize(file, swap);
		ok = transaction && llist_add_node(block->transactions,
			transaction, ADD_NODE_REAR) == 0;
		if (!ok)
			transaction_destroy(transaction);
	}
	if (ok)
		return (block);
	block_destroy(block);
	return (NULL);
}

/**
 * blockchain_deserialize - Deserializes a Blockchain from a file
 *
 * @path: Path to the file to load the Blockchain from
 *
 * Return: Pointer to the deserialized Blockchain, or NULL upon failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file = path ? fopen(path, "rb") : NULL;
	blockchain_t *blockchain = calloc(1, sizeof(*blockchain));
	block_t *block;
	uint8_t header[8];
	uint32_t one = 1, nb_blocks, nb_unspent;
	int swap, ok;

	ok = file && blockchain && read_bytes(file, header, 8, 0) &&
		memcmp(header, HBLK_MAGIC, 4) == 0 &&
		memcmp(header + 4, HBLK_VERSION, 3) == 0 &&
		(header[7] == 1 || header[7] == 2);
	if (ok)
	{
		swap = header[7] != (*(uint8_t *)&one ? 1 : 2);
		blockchain->chain = llist_create(MT_SUPPORT_FALSE);
		ok = blockchain->chain && read_bytes(file, &nb_blocks, 4, swap) &&
			read_bytes(file, &nb_unspent, 4, swap);
	}
	for (; ok && nb_blocks; nb_blocks--)
	{
		block = block_deserialize(file, swap);
		ok = block && llist_add_node(blockchain->chain, block,
			ADD_NODE_REAR) == 0;
		if (!ok)
			block_destroy(block);
	}
	if (ok)
		blockchain->unspent = items_deserialize(file, nb_unspent,
			ITEM_UNSPENT, swap);
	if (file)
		fclose(file);
	if (ok && blockchain->unspent)
		return (blockchain);
	blockchain_destroy(blockchain);
	return (NULL);
}
