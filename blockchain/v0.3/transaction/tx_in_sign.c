#include "transaction.h"

/**
 * unspent_match - Checks if an unspent output is referenced by an input
 *
 * @unspent: Pointer to the unspent transaction output
 * @in:      Pointer to the transaction input
 *
 * Return: 1 if @in refers to @unspent, 0 otherwise
 */
static int unspent_match(unspent_tx_out_t *unspent, tx_in_t *in)
{
	return (!memcmp(unspent->block_hash, in->block_hash,
			SHA256_DIGEST_LENGTH) &&
		!memcmp(unspent->tx_id, in->tx_id, SHA256_DIGEST_LENGTH) &&
		!memcmp(unspent->out.hash, in->tx_out_hash,
			SHA256_DIGEST_LENGTH));
}

/**
 * tx_in_sign - Signs a transaction input, given the transaction ID it is from
 *
 * @in:          Pointer to the transaction input to sign
 * @tx_id:       ID (hash) of the transaction the input is stored in
 * @sender:      Private key of the receiver of the coins contained in the
 *               transaction output referenced by the transaction input
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: Pointer to the resulting signature, or NULL upon failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
	EC_KEY const *sender, llist_t *all_unspent)
{
	uint8_t pub[EC_PUB_LEN];
	unspent_tx_out_t *unspent;

	if (!in || !tx_id || !sender || !all_unspent)
		return (NULL);

	unspent = llist_find_node(all_unspent, (node_ident_t)unspent_match, in);
	if (!unspent)
		return (NULL);

	if (!ec_to_pub(sender, pub) ||
	    memcmp(pub, unspent->out.pub, EC_PUB_LEN) != 0)
		return (NULL);

	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
		return (NULL);

	return (&in->sig);
}
