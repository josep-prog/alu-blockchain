#include <stdio.h>

#include "blockchain.h"
#include "provided/endianness.h"

/**
 * block_deserialize - Reads a single Block from a file
 *
 * @file: Stream to read the Block from
 * @swap: Non-zero if the endianness of the file differs from the host's
 *
 * Return: Pointer to the allocated Block, or NULL upon failure
 */
static block_t *block_deserialize(FILE *file, int swap)
{
	block_t *block;

	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	if (fread(&block->info, sizeof(block->info), 1, file) != 1 ||
	    fread(&block->data.len, sizeof(block->data.len), 1, file) != 1)
	{
		free(block);
		return (NULL);
	}
	if (swap)
	{
		SWAPENDIAN(block->info.index);
		SWAPENDIAN(block->info.difficulty);
		SWAPENDIAN(block->info.timestamp);
		SWAPENDIAN(block->info.nonce);
		SWAPENDIAN(block->data.len);
	}
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
	    fread(block->data.buffer, 1, block->data.len, file) !=
	    block->data.len ||
	    fread(block->hash, sizeof(block->hash), 1, file) != 1)
	{
		free(block);
		return (NULL);
	}

	return (block);
}

/**
 * header_deserialize - Reads and checks the header of a serialized Blockchain
 *
 * @file:  Stream to read the header from
 * @swap:  Set to non-zero if the file endianness differs from the host's
 * @size:  Set to the number of Blocks stored in the file
 *
 * Return: 0 upon success, -1 upon failure
 */
static int header_deserialize(FILE *file, int *swap, uint32_t *size)
{
	uint8_t header[8];

	if (fread(header, 1, sizeof(header), file) != sizeof(header) ||
	    memcmp(header, HBLK_MAGIC, 4) != 0 ||
	    memcmp(header + 4, HBLK_VERSION, 3) != 0 ||
	    (header[7] != 1 && header[7] != 2) ||
	    fread(size, sizeof(*size), 1, file) != 1)
		return (-1);

	*swap = header[7] != _get_endianness();
	if (*swap)
		_swap_endian(size, sizeof(*size));

	return (0);
}

/**
 * blocks_deserialize - Reads Blocks from a file and adds them to a chain
 *
 * @file:  Stream to read the Blocks from
 * @chain: List in which the Blocks are stored
 * @size:  Number of Blocks to read
 * @swap:  Non-zero if the endianness of the file differs from the host's
 *
 * Return: 0 upon success, -1 upon failure
 */
static int blocks_deserialize(FILE *file, llist_t *chain, uint32_t size,
	int swap)
{
	block_t *block;
	uint32_t i;

	for (i = 0; i < size; i++)
	{
		block = block_deserialize(file, swap);
		if (!block)
			return (-1);
		if (llist_add_node(chain, block, ADD_NODE_REAR) == -1)
		{
			free(block);
			return (-1);
		}
	}

	return (0);
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
	FILE *file;
	blockchain_t *blockchain = NULL;
	uint32_t size;
	int swap;

	if (!path)
		return (NULL);
	file = fopen(path, "rb");
	if (!file)
		return (NULL);

	if (header_deserialize(file, &swap, &size) == -1)
		goto fail;
	blockchain = calloc(1, sizeof(*blockchain));
	if (!blockchain)
		goto fail;
	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain ||
	    blocks_deserialize(file, blockchain->chain, size, swap) == -1)
		goto fail;

	fclose(file);
	return (blockchain);

fail:
	fclose(file);
	if (blockchain)
		llist_destroy(blockchain->chain, 1, free);
	free(blockchain);
	return (NULL);
}
