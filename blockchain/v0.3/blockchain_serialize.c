#include <stdio.h>

#include "blockchain.h"

#define WRITE(ptr, size, file) (fwrite(ptr, 1, size, file) == (size_t)(size))

/**
 * tx_serialize - Writes a single transaction into a file
 *
 * @transaction: Pointer to the transaction to serialize
 * @idx:         Index of the transaction in its list (unused)
 * @file:        Stream to write the transaction to
 *
 * Return: 0 upon success, 1 upon failure (stops the iteration)
 */
static int tx_serialize(transaction_t *transaction, unsigned int idx,
	FILE *file)
{
	int32_t i, nb_in, nb_out;
	tx_in_t *in;
	tx_out_t *out;
	uint8_t sig_len;

	(void)idx;
	nb_in = llist_size(transaction->inputs);
	nb_out = llist_size(transaction->outputs);
	if (nb_in < 0 || nb_out < 0 ||
	    !WRITE(transaction->id, SHA256_DIGEST_LENGTH, file) ||
	    !WRITE(&nb_in, 4, file) || !WRITE(&nb_out, 4, file))
		return (1);

	for (i = 0; i < nb_in; i++)
	{
		in = llist_get_node_at(transaction->inputs, i);
		sig_len = in ? (uint8_t)in->sig.len : 0;
		if (!in || !WRITE(in, TX_IN_REF_LEN, file) ||
		    !WRITE(in->sig.sig, SIG_MAX_LEN, file) ||
		    !WRITE(&sig_len, 1, file))
			return (1);
	}
	for (i = 0; i < nb_out; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out || !WRITE(&out->amount, 4, file) ||
		    !WRITE(out->pub, EC_PUB_LEN, file) ||
		    !WRITE(out->hash, SHA256_DIGEST_LENGTH, file))
			return (1);
	}

	return (0);
}

/**
 * block_serialize - Writes a single Block into a file
 *
 * @block: Pointer to the Block to serialize
 * @idx:   Index of the Block in the chain (unused)
 * @file:  Stream to write the Block to
 *
 * Return: 0 upon success, 1 upon failure (stops the iteration)
 */
static int block_serialize(block_t *block, unsigned int idx, FILE *file)
{
	int32_t nb_tx = -1;

	(void)idx;
	if (block->transactions)
		nb_tx = llist_size(block->transactions);

	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
	    !WRITE(&block->info, sizeof(block->info), file) ||
	    !WRITE(&block->data.len, 4, file) ||
	    !WRITE(block->data.buffer, block->data.len, file) ||
	    !WRITE(block->hash, SHA256_DIGEST_LENGTH, file) ||
	    !WRITE(&nb_tx, 4, file))
		return (1);

	if (nb_tx > 0 && llist_for_each(block->transactions,
		(node_func_t)tx_serialize, file) != 0)
		return (1);

	return (0);
}

/**
 * unspent_serialize - Writes a single unspent transaction output into a file
 *
 * @unspent: Pointer to the unspent transaction output to serialize
 * @idx:     Index of the unspent output in its list (unused)
 * @file:    Stream to write the unspent output to
 *
 * Return: 0 upon success, 1 upon failure (stops the iteration)
 */
static int unspent_serialize(unspent_tx_out_t *unspent, unsigned int idx,
	FILE *file)
{
	(void)idx;

	return (!WRITE(unspent->block_hash, SHA256_DIGEST_LENGTH, file) ||
		!WRITE(unspent->tx_id, SHA256_DIGEST_LENGTH, file) ||
		!WRITE(&unspent->out.amount, 4, file) ||
		!WRITE(unspent->out.pub, EC_PUB_LEN, file) ||
		!WRITE(unspent->out.hash, SHA256_DIGEST_LENGTH, file));
}

/**
 * blockchain_serialize - Serializes a Blockchain into a file
 *
 * @blockchain: Pointer to the Blockchain to be serialized
 * @path:       Path to the file to serialize the Blockchain into
 *
 * Return: 0 upon success, -1 upon failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *file;
	uint32_t one = 1;
	uint8_t endian = *(uint8_t *)&one ? 1 : 2;
	int32_t nb_blocks, nb_unspent = 0;
	int failed;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);

	nb_blocks = llist_size(blockchain->chain);
	if (blockchain->unspent)
		nb_unspent = llist_size(blockchain->unspent);
	if (nb_blocks < 0 || nb_unspent < 0)
		return (-1);

	file = fopen(path, "wb");
	if (!file)
		return (-1);

	failed = !WRITE(HBLK_MAGIC, 4, file) || !WRITE(HBLK_VERSION, 3, file) ||
		!WRITE(&endian, 1, file) || !WRITE(&nb_blocks, 4, file) ||
		!WRITE(&nb_unspent, 4, file) ||
		(nb_blocks > 0 && llist_for_each(blockchain->chain,
			(node_func_t)block_serialize, file) != 0) ||
		(nb_unspent > 0 && llist_for_each(blockchain->unspent,
			(node_func_t)unspent_serialize, file) != 0);

	if (fclose(file) != 0 || failed)
		return (-1);

	return (0);
}
