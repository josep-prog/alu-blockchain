#include "hblk_crypto.h"

/**
 * sha256 - Computes the SHA256 (256-bit) hash of a sequence of bytes
 * @s: Sequence of bytes to be hashed
 * @len: Number of bytes to hash in @s
 * @digest: Buffer in which to store the resulting hash
 *
 * Return: A pointer to @digest, or NULL on failure
 */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);

	SHA256((unsigned char const *)s, len, digest);

	return (digest);
}
