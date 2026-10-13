#include <stdio.h>

#include "blockchain.h"
#include "provided/endianness.h"

/**
 * block_serialize - Writes a single Block into a file
 *
 * @block: Pointer to the Block to serialize
 * @index: Index of the Block in the chain (unused)
 * @file:  Stream to write the Block to
 *
 * Return: 0 upon success, 1 upon failure (stops the iteration)
 */
static int block_serialize(block_t *block, unsigned int index, FILE *file)
{
	(void)index;

	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (1);
	if (fwrite(&block->info, sizeof(block->info), 1, file) != 1 ||
	    fwrite(&block->data.len, sizeof(block->data.len), 1, file) != 1 ||
	    fwrite(block->data.buffer, 1, block->data.len, file) !=
	    block->data.len ||
	    fwrite(block->hash, sizeof(block->hash), 1, file) != 1)
		return (1);

	return (0);
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
	uint8_t endian = _get_endianness();
	int32_t size;
	int failed;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);

	size = llist_size(blockchain->chain);
	if (size < 0)
		return (-1);

	file = fopen(path, "wb");
	if (!file)
		return (-1);

	failed = fwrite(HBLK_MAGIC, 1, 4, file) != 4 ||
		fwrite(HBLK_VERSION, 1, 3, file) != 3 ||
		fwrite(&endian, 1, 1, file) != 1 ||
		fwrite(&size, sizeof(size), 1, file) != 1 ||
		llist_for_each(blockchain->chain,
			(node_func_t)block_serialize, file) != 0;

	if (fclose(file) != 0 || failed)
		return (-1);

	return (0);
}
