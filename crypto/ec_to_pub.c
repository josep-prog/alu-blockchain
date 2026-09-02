#include "hblk_crypto.h"

/**
 * ec_to_pub - Extracts the public key from an EC_KEY opaque structure
 * @key: Pointer to the EC_KEY structure to retrieve the public key from
 * @pub: Buffer in which to store the extracted (uncompressed) public key
 *
 * Return: A pointer to @pub, or NULL upon failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	EC_GROUP const *group;
	EC_POINT const *point;
	BN_CTX *ctx;
	size_t len;

	if (!key)
		return (NULL);

	group = EC_KEY_get0_group(key);
	point = EC_KEY_get0_public_key(key);
	if (!group || !point)
		return (NULL);

	ctx = BN_CTX_new();
	if (!ctx)
		return (NULL);

	len = EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED,
			pub, EC_PUB_LEN, ctx);
	BN_CTX_free(ctx);

	if (len != EC_PUB_LEN)
		return (NULL);

	return (pub);
}
