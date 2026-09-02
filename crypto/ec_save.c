#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

#define EC_PATH_MAX 4096

/**
 * ec_save - Saves an existing EC key pair on the disk
 * @key: Pointer to the EC key pair to be saved on disk
 * @folder: Path to the folder in which to save the keys
 *
 * Return: 1 upon success, or 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char path[EC_PATH_MAX];
	FILE *fp;

	if (!key || !folder)
		return (0);

	if (mkdir(folder, 0755) != 0 && errno != EEXIST)
		return (0);

	snprintf(path, sizeof(path), "%s/key.pem", folder);
	fp = fopen(path, "w");
	if (!fp)
		return (0);
	if (!PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	snprintf(path, sizeof(path), "%s/key_pub.pem", folder);
	fp = fopen(path, "w");
	if (!fp)
		return (0);
	if (!PEM_write_EC_PUBKEY(fp, key))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	return (1);
}
