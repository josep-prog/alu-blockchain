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
 * input_check - Checks a single input of a transaction
 *
 * @transaction: Pointer to the transaction the input belongs to
 * @idx:         Index of the input to check in the transaction
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Description: The input must refer to an unspent output, must not be used
 * twice in the transaction, and its signature must be verified using the
 * public key stored in the referenced unspent output.
 *
 * Return: Pointer to the referenced unspent output, or NULL if invalid
 */
static unspent_tx_out_t *input_check(transaction_t const *transaction,
	int idx, llist_t *all_unspent)
{
	tx_in_t *in, *other;
	unspent_tx_out_t *unspent;
	EC_KEY *key;
	int i, verified;

	in = llist_get_node_at(transaction->inputs, idx);
	if (!in)
		return (NULL);
	for (i = 0; i < idx; i++)
	{
		other = llist_get_node_at(transaction->inputs, i);
		if (other && memcmp(other, in, TX_IN_REF_LEN) == 0)
			return (NULL);
	}

	unspent = llist_find_node(all_unspent, (node_ident_t)unspent_match, in);
	if (!unspent)
		return (NULL);

	key = ec_from_pub(unspent->out.pub);
	if (!key)
		return (NULL);
	verified = ec_verify(key, transaction->id, SHA256_DIGEST_LENGTH,
		&in->sig);
	EC_KEY_free(key);

	return (verified ? unspent : NULL);
}

/**
 * transaction_is_valid - Checks whether a transaction is valid
 *
 * @transaction: Pointer to the transaction to verify
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 1 if the transaction is valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
	llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	unspent_tx_out_t *unspent;
	tx_out_t *out;
	uint64_t total_in = 0, total_out = 0;
	int i, nb_in, nb_out;

	if (!transaction || !all_unspent)
		return (0);
	if (!transaction_hash(transaction, hash) ||
	    memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	nb_in = llist_size(transaction->inputs);
	nb_out = llist_size(transaction->outputs);
	if (nb_in < 1 || nb_out < 1)
		return (0);

	for (i = 0; i < nb_in; i++)
	{
		unspent = input_check(transaction, i, all_unspent);
		if (!unspent)
			return (0);
		total_in += unspent->out.amount;
	}
	for (i = 0; i < nb_out; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out)
			return (0);
		total_out += out->amount;
	}

	return (total_in == total_out);
}
