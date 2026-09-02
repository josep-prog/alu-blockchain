#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/pem.h>
#include <openssl/obj_mac.h>

/* The elliptic curve used throughout this project */
#define EC_CURVE NID_secp256k1

/* Length (in bytes) of an uncompressed EC public key on secp256k1 */
#define EC_PUB_LEN 65

/* Maximum length (in bytes) of a DER-encoded ECDSA signature on secp256k1 */
#define SIG_MAX_LEN 72

/* Filenames used by ec_save()/ec_load() to store the key pair on disk */
#define PRI_FILENAME "key.pem"
#define PUB_FILENAME "key_pub.pem"

/**
 * struct sig_s - Signature structure
 *
 * @sig: Buffer containing the actual signature
 * @len: Length of @sig, in bytes
 */
typedef struct sig_s
{
	uint8_t sig[SIG_MAX_LEN];
	size_t  len;
} sig_t;

uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH]);

EC_KEY *ec_create(void);
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);
int ec_save(EC_KEY *key, char const *folder);
EC_KEY *ec_load(char const *folder);
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg,
		size_t msglen, sig_t *sig);
int ec_verify(EC_KEY const *key, uint8_t const *msg,
		size_t msglen, sig_t const *sig);

#endif /* HBLK_CRYPTO_H */
