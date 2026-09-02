#include <stdio.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

#define EC_PATH_MAX 4096

/**
 * _load_pub_key - Loads the public key stored in <folder>/key_pub.pem
 * @folder: Path to the folder from which to load the public key
 *
 * Return: A pointer to the loaded EC_KEY, or NULL upon failure
 */
static EC_KEY *_load_pub_key(char const *folder)
{
	char path[EC_PATH_MAX];
	FILE *fp;
	EC_KEY *pub_key;

	snprintf(path, sizeof(path), "%s/" PUB_FILENAME, folder);
	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	pub_key = PEM_read_EC_PUBKEY(fp, NULL, NULL, NULL);
	fclose(fp);

	return (pub_key);
}

/**
 * ec_load - Loads an EC key pair from the disk
 * @folder: Path to the folder from which to load the keys
 *
 * Return: A pointer to the created EC key pair upon success,
 *         or NULL upon failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[EC_PATH_MAX];
	FILE *fp;
	EC_KEY *key, *pub_key;

	if (!folder)
		return (NULL);

	snprintf(path, sizeof(path), "%s/" PRI_FILENAME, folder);
	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	if (!key)
		return (NULL);

	pub_key = _load_pub_key(folder);
	if (!pub_key || !EC_KEY_set_public_key(key, EC_KEY_get0_public_key(pub_key)))
	{
		EC_KEY_free(pub_key);
		EC_KEY_free(key);
		return (NULL);
	}
	EC_KEY_free(pub_key);

	return (key);
}
