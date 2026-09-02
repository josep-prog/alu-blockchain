#include <openssl/ecdsa.h>
#include "hblk_crypto.h"

/**
 * ec_sign - Signs a given set of bytes, using a given EC_KEY private key
 * @key: Pointer to the EC_KEY structure containing the private key
 *       to be used to perform the signature
 * @msg: Pointer to the bytes to be signed
 * @msglen: Number of bytes to sign in @msg
 * @sig: Structure in which to store the signature
 *
 * Return: A pointer to the signature buffer (@sig->sig) upon success,
 *         or NULL upon failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg,
		size_t msglen, sig_t *sig)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	unsigned int len;

	if (!key || !msg || !sig)
		return (NULL);

	if (!SHA256((unsigned char const *)msg, msglen, hash))
		return (NULL);

	if (!ECDSA_sign(0, hash, SHA256_DIGEST_LENGTH, sig->sig, &len,
				(EC_KEY *)key))
		return (NULL);

	sig->len = len;

	return (sig->sig);
}
