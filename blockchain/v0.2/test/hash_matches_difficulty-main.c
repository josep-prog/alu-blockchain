#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint32_t difficulty;

	memset(hash, 0xFF, sizeof(hash));
	hash[0] = 0x00;
	hash[1] = 0x00;
	hash[2] = 0x1F; /* 19 leading zero bits */

	for (difficulty = 17; difficulty <= 21; difficulty++)
	{
		if (hash_matches_difficulty(hash, difficulty))
			printf("Difficulty %u: matches\n", difficulty);
		else
			printf("Difficulty %u: doesn't match\n", difficulty);
	}

	return (EXIT_SUCCESS);
}
