#include "transaction.h"

/**
 * is_spent - Checks if an unspent output is consumed by a list of transactions
 *
 * @unspent:      Pointer to the unspent transaction output to check
 * @transactions: List of validated transactions
 *
 * Return: 1 if an input of @transactions refers to @unspent, 0 otherwise
 */
static int is_spent(unspent_tx_out_t const *unspent, llist_t *transactions)
{
	transaction_t *transaction;
	tx_in_t *in;
	int i, j, nb_tx, nb_in;

	nb_tx = llist_size(transactions);
	for (i = 0; i < nb_tx; i++)
	{
		transaction = llist_get_node_at(transactions, i);
		nb_in = transaction ? llist_size(transaction->inputs) : 0;
		for (j = 0; j < nb_in; j++)
		{
			in = llist_get_node_at(transaction->inputs, j);
			if (in &&
			    !memcmp(unspent->block_hash, in->block_hash,
				SHA256_DIGEST_LENGTH) &&
			    !memcmp(unspent->tx_id, in->tx_id,
				SHA256_DIGEST_LENGTH) &&
			    !memcmp(unspent->out.hash, in->tx_out_hash,
				SHA256_DIGEST_LENGTH))
				return (1);
		}
	}

	return (0);
}

/**
 * outputs_add - Appends the outputs of a transaction to a list of unspent
 *
 * @transaction: Pointer to the transaction which outputs must be added
 * @block_hash:  Hash of the Block that contains @transaction
 * @unspent:     List in which the unspent outputs are stored
 *
 * Return: 0 upon success, -1 upon failure
 */
static int outputs_add(transaction_t *transaction,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *unspent)
{
	unspent_tx_out_t *node;
	int i, nb_out = llist_size(transaction->outputs);

	for (i = 0; i < nb_out; i++)
	{
		node = unspent_tx_out_create(block_hash, transaction->id,
			llist_get_node_at(transaction->outputs, i));
		if (!node)
			return (-1);
		if (llist_add_node(unspent, node, ADD_NODE_REAR) == -1)
		{
			free(node);
			return (-1);
		}
	}

	return (0);
}

/**
 * unspent_build - Builds the new list of unspent transaction outputs
 *
 * @transactions: List of validated transactions
 * @block_hash:   Hash of the validated Block that contains @transactions
 * @all_unspent:  Current list of unspent transaction outputs
 * @kept:         Set to the number of nodes taken from @all_unspent
 *
 * Return: Pointer to the new list, or NULL upon failure
 */
static llist_t *unspent_build(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent,
	int *kept)
{
	llist_t *unspent;
	unspent_tx_out_t *node;
	int i, size;

	unspent = llist_create(MT_SUPPORT_FALSE);
	if (!unspent)
		return (NULL);

	size = llist_size(all_unspent);
	for (i = 0; i < size; i++)
	{
		node = llist_get_node_at(all_unspent, i);
		if (!node || is_spent(node, transactions))
			continue;
		if (llist_add_node(unspent, node, ADD_NODE_REAR) == -1)
			return (unspent);
		(*kept)++;
	}

	size = llist_size(transactions);
	for (i = 0; i < size; i++)
	{
		if (outputs_add(llist_get_node_at(transactions, i),
			block_hash, unspent) == -1)
			return (unspent);
	}

	*kept = -1 - *kept;
	return (unspent);
}

/**
 * update_unspent - Updates the list of all unspent transaction outputs,
 * given a list of processed transactions
 *
 * @transactions: List of validated transactions
 * @block_hash:   Hash of the validated Block that contains @transactions
 * @all_unspent:  Current list of unspent transaction outputs
 *
 * Description: Upon success, @all_unspent is deleted. Upon failure, it is
 * left untouched.
 *
 * Return: Pointer to the new list of unspent outputs, or NULL upon failure
 */
llist_t *update_unspent(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent)
{
	llist_t *unspent;
	unspent_tx_out_t *node;
	int i, size, kept = 0;

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);

	unspent = unspent_build(transactions, block_hash, all_unspent, &kept);
	if (!unspent)
		return (NULL);
	if (kept >= 0)
	{
		/* Failure: only free the nodes created for the new list */
		size = llist_size(unspent);
		for (i = kept; i < size; i++)
			free(llist_get_node_at(unspent, i));
		llist_destroy(unspent, 0, NULL);
		return (NULL);
	}

	size = llist_size(all_unspent);
	for (i = 0; i < size; i++)
	{
		node = llist_get_node_at(all_unspent, i);
		if (node && is_spent(node, transactions))
			free(node);
	}
	llist_destroy(all_unspent, 0, NULL);

	return (unspent);
}
