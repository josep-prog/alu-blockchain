#include <stdio.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

#define EC_PATH_MAX 4096

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
	EC_KEY *key = NULL;

	if (!folder)
		return (NULL);

	snprintf(path, sizeof(path), "%s/key.pem", folder);
	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	if (!key)
		return (NULL);

	snprintf(path, sizeof(path), "%s/key_pub.pem", folder);
	fp = fopen(path, "r");
	if (!fp)
	{
		EC_KEY_free(key);
		return (NULL);
	}
	if (!PEM_read_EC_PUBKEY(fp, &key, NULL, NULL))
	{
		fclose(fp);
		EC_KEY_free(key);
		return (NULL);
	}
	fclose(fp);

	return (key);
}
