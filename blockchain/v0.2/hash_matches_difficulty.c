#include "blockchain.h"

/**
 * hash_matches_difficulty - Checks whether a hash matches a difficulty
 *
 * @hash:       Hash to check
 * @difficulty: Minimum number of leading zero bits the hash must have
 *
 * Return: 1 if the difficulty is respected, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
	uint32_t difficulty)
{
	uint32_t i, bits = 0;
	uint8_t mask;

	if (!hash || difficulty > SHA256_DIGEST_LENGTH * 8)
		return (0);

	for (i = 0; i < SHA256_DIGEST_LENGTH && bits < difficulty; i++)
	{
		for (mask = 0x80; mask; mask >>= 1)
		{
			if (hash[i] & mask)
				return (bits >= difficulty);
			bits++;
		}
	}

	return (bits >= difficulty);
}
