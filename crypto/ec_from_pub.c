#include "hblk_crypto.h"

/**
 * _set_pub_key - Sets the public key of an EC_KEY structure from raw bytes
 * @key: Pointer to the EC_KEY structure to set the public key on
 * @pub: Buffer containing the public key
 *
 * Return: 1 upon success, or 0 upon failure
 */
static int _set_pub_key(EC_KEY *key, uint8_t const pub[EC_PUB_LEN])
{
	EC_GROUP const *group;
	EC_POINT *point;
	BN_CTX *ctx;
	int ret;

	group = EC_KEY_get0_group(key);
	point = EC_POINT_new(group);
	if (!point)
		return (0);

	ctx = BN_CTX_new();
	if (!ctx)
	{
		EC_POINT_free(point);
		return (0);
	}

	ret = EC_POINT_oct2point(group, point, pub, EC_PUB_LEN, ctx) &&
		EC_KEY_set_public_key(key, point);

	BN_CTX_free(ctx);
	EC_POINT_free(point);

	return (ret);
}

/**
 * ec_from_pub - Creates an EC_KEY structure given a public key
 * @pub: Buffer containing the public key to be converted
 *
 * Return: A pointer to the created EC_KEY structure upon success,
 *         or NULL upon failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key;

	if (!pub)
		return (NULL);

	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	if (!_set_pub_key(key, pub))
	{
		EC_KEY_free(key);
		return (NULL);
	}

	return (key);
}
