#include "transaction.h"

/**
 * tx_out_create - Allocates and initializes a transaction output
 *
 * @amount: Amount of the transaction
 * @pub:    Public key of the transaction receiver
 *
 * Return: Pointer to the created transaction output, or NULL upon failure
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *out;

	if (!pub)
		return (NULL);

	out = calloc(1, sizeof(*out));
	if (!out)
		return (NULL);

	out->amount = amount;
	memcpy(out->pub, pub, EC_PUB_LEN);
	if (!SHA256((unsigned char const *)out,
		sizeof(out->amount) + sizeof(out->pub), out->hash))
	{
		free(out);
		return (NULL);
	}

	return (out);
}
