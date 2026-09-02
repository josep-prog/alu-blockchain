#include <openssl/ecdsa.h>
#include "hblk_crypto.h"

/**
 * ec_verify - Verifies the signature of a given set of bytes,
 *             using a given EC_KEY public key
 * @key: Pointer to the EC_KEY structure containing the public key
 *       to be used to verify the signature
 * @msg: Pointer to the bytes to verify the signature of
 * @msglen: Number of bytes to verify in @msg
 * @sig: Pointer to the signature to be checked
 *
 * Return: 1 if the signature is valid, or 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg,
		size_t msglen, sig_t const *sig)
{
	if (!key || !msg || !sig)
		return (0);

	if (ECDSA_verify(0, msg, (int)msglen, sig->sig, (int)sig->len,
				(EC_KEY *)key) != 1)
		return (0);

	return (1);
}
