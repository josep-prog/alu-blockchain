#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

void _blockchain_print_brief(blockchain_t const *blockchain);

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *blockchain;
	block_t *block;

	blockchain = blockchain_create();
	block = llist_get_head(blockchain->chain);

	block = block_create(block, (int8_t *)"Holberton", 9);
	block->info.difficulty = 20;
	block_mine(block);
	llist_add_node(blockchain->chain, block, ADD_NODE_REAR);

	if (block_is_valid(block, llist_get_head(blockchain->chain)) != 0)
		fprintf(stderr, "Block invalid\n");

	_blockchain_print_brief(blockchain);
	blockchain_destroy(blockchain);

	return (EXIT_SUCCESS);
}
