# holbertonschool-blockchain

Low-level C building blocks for a Blockchain: hashing and elliptic-curve
digital signatures, backed by OpenSSL's `libcrypto`.

## crypto

The `crypto/` folder holds a static library, `libhblk_crypto.a`, exposing:

| Function      | Purpose                                                   |
|---------------|------------------------------------------------------------|
| `sha256`      | Compute the SHA256 digest of a buffer                     |
| `ec_create`   | Generate a new secp256k1 EC key pair                       |
| `ec_to_pub`   | Extract the uncompressed public key from an `EC_KEY`      |
| `ec_from_pub` | Rebuild an `EC_KEY` (public part only) from raw bytes     |
| `ec_save`     | Save an `EC_KEY` pair to `<folder>/key.pem` (+ `_pub.pem`)|
| `ec_load`     | Load an `EC_KEY` pair back from those two PEM files        |
| `ec_sign`     | Sign a buffer (ECDSA over SHA256) with a private key       |
| `ec_verify`   | Verify an ECDSA signature against a public key             |

Build the library:

```sh
cd crypto
make
```

This produces `libhblk_crypto.a`. All prototypes and the `sig_t` type live
in `crypto/hblk_crypto.h`.

### Building/running a test

Each function has a matching example program in `crypto/test/`, e.g.:

```sh
gcc -Wall -Wextra -Werror -pedantic -I. \
    -o sha256-test test/sha256-main.c provided/_print_hex_buffer.c sha256.c \
    -lssl -lcrypto
./sha256-test Holberton
```

### Note on OpenSSL versions

This project's `EC_KEY`-based API (`EC_KEY_new_by_curve_name`,
`EC_KEY_generate_key`, etc.) was deprecated in OpenSSL 3.0 in favor of the
generic `EVP_PKEY` API. The Makefile adds `-Wno-deprecated-declarations` so
the code still builds warning-free with `-Werror` on modern systems
(OpenSSL 3.x) as well as on the older OpenSSL the project originally
targeted.

## Concepts

- **Hash algorithm**: a one-way function that turns an arbitrary amount of
  data into a fixed-size fingerprint. Same input always gives the same
  output; changing even one bit of input scrambles the output completely;
  and you can't feasibly go from the output back to the input, or find two
  different inputs that hash to the same output.
- **SHA**: Secure Hash Algorithm — a family of hash functions (SHA-1,
  SHA-2, SHA-3) published by NIST. This project uses SHA-256, part of the
  SHA-2 family, which always produces a 256-bit (32-byte) digest.
- **Hashes in a Blockchain**: each block stores the hash of the previous
  block. Since a hash changes completely if any byte of its input changes,
  altering an old block would change its hash, which would break the
  "previous hash" link stored in the next block, and so on all the way to
  the tip of the chain — so tampering is immediately detectable, which is
  why a Blockchain is considered "unbreakable" (in practice, you'd have to
  redo the sequential work for every block after the one you changed,
  faster than the rest of the network, which is what makes it practically
  infeasible rather than mathematically impossible).
- **Asymmetric cryptography**: uses a mathematically linked key pair — a
  private key kept secret and a public key that can be shared freely.
  Data encrypted/signed with one key can only be decrypted/verified with
  the other, and the private key cannot feasibly be derived from the
  public one.
- **Asymmetric cryptography in cryptocurrencies**: a wallet address is
  derived from a public key; owning the matching private key is what lets
  someone spend the coins associated with that address. No password or
  central authority is involved — possession of the private key is
  ownership.
- **ECC**: Elliptic Curve Cryptography — asymmetric cryptography built on
  the algebraic structure of elliptic curves over finite fields. It gives
  the same security as older schemes like RSA with much shorter keys,
  because there's no known efficient way to solve the underlying elliptic
  curve discrete logarithm problem.
- **ECDSA**: Elliptic Curve Digital Signature Algorithm — the digital
  signature scheme built on top of ECC. This project uses it over the
  `secp256k1` curve, the same curve Bitcoin uses.
- **Digital signature**: a value computed from a message and a private key
  such that anyone holding the corresponding public key can verify the
  signature matches both that exact message and that private key, without
  ever seeing the private key itself. Changing a single byte of the
  message invalidates the signature.
- **Digital signatures in cryptocurrencies**: every transaction is signed
  with the sender's private key. Nodes verify the signature against the
  sender's public key/address before accepting the transaction, proving
  the funds' owner authorized it, and that the transaction wasn't altered
  in transit — all without the private key ever being transmitted or
  revealed.
