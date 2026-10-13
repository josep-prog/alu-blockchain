#include "transaction.h"

/**
 * tx_free - Frees a transaction and its content
 *
 * @transaction: Pointer to the transaction to free
 */
static void tx_free(transaction_t *transaction)
{
	if (transaction->inputs)
		llist_destroy(transaction->inputs, 1, free);
	if (transaction->outputs)
		llist_destroy(transaction->outputs, 1, free);
	free(transaction);
}

/**
 * inputs_select - Creates inputs from the unspent outputs of the sender
 *
 * @inputs:      List in which the created inputs are stored
 * @all_unspent: List of all unspent transaction outputs to date
 * @pub:         Public key of the sender
 * @amount:      Amount to gather
 * @total:       Set to the total amount of the selected unspent outputs
 *
 * Return: 0 upon success, -1 upon failure or if the sender lacks coins
 */
static int inputs_select(llist_t *inputs, llist_t *all_unspent,
	uint8_t const pub[EC_PUB_LEN], uint32_t amount, uint64_t *total)
{
	unspent_tx_out_t *unspent;
	tx_in_t *in;
	int i, size = llist_size(all_unspent);

	*total = 0;
	for (i = 0; i < size && *total < amount; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent || memcmp(unspent->out.pub, pub, EC_PUB_LEN) != 0)
			continue;
		in = tx_in_create(unspent);
		if (!in)
			return (-1);
		if (llist_add_node(inputs, in, ADD_NODE_REAR) == -1)
		{
			free(in);
			return (-1);
		}
		*total += unspent->out.amount;
	}

	return (*total < amount || llist_size(inputs) < 1 ? -1 : 0);
}

/**
 * output_add - Creates a transaction output and adds it to a list
 *
 * @outputs: List in which the created output is stored
 * @amount:  Amount of the output
 * @pub:     Public key of the receiver of the output
 *
 * Return: 0 upon success, -1 upon failure
 */
static int output_add(llist_t *outputs, uint32_t amount,
	uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *out;

	out = tx_out_create(amount, pub);
	if (!out)
		return (-1);
	if (llist_add_node(outputs, out, ADD_NODE_REAR) == -1)
	{
		free(out);
		return (-1);
	}

	return (0);
}

/**
 * inputs_sign - Signs all the inputs of a transaction
 *
 * @transaction: Pointer to the transaction which inputs must be signed
 * @sender:      Private key of the transaction sender
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 0 upon success, -1 upon failure
 */
static int inputs_sign(transaction_t *transaction, EC_KEY const *sender,
	llist_t *all_unspent)
{
	int i, size = llist_size(transaction->inputs);

	for (i = 0; i < size; i++)
	{
		if (!tx_in_sign(llist_get_node_at(transaction->inputs, i),
			transaction->id, sender, all_unspent))
			return (-1);
	}

	return (0);
}

/**
 * transaction_create - Creates a transaction
 *
 * @sender:      Private key of the transaction sender
 * @receiver:    Public key of the transaction receiver
 * @amount:      Amount to send
 * @all_unspent: List of all the unspent outputs to date
 *
 * Return: Pointer to the created transaction, or NULL upon failure
 */
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver,
	uint32_t amount, llist_t *all_unspent)
{
	uint8_t from[EC_PUB_LEN], to[EC_PUB_LEN];
	transaction_t *transaction;
	uint64_t total;

	if (!sender || !receiver || !all_unspent ||
	    !ec_to_pub(sender, from) || !ec_to_pub(receiver, to))
		return (NULL);

	transaction = calloc(1, sizeof(*transaction));
	if (!transaction)
		return (NULL);
	transaction->inputs = llist_create(MT_SUPPORT_FALSE);
	transaction->outputs = llist_create(MT_SUPPORT_FALSE);

	if (!transaction->inputs || !transaction->outputs ||
	    inputs_select(transaction->inputs, all_unspent, from, amount,
		&total) == -1 ||
	    output_add(transaction->outputs, amount, to) == -1 ||
	    (total > amount && output_add(transaction->outputs,
		(uint32_t)(total - amount), from) == -1) ||
	    !transaction_hash(transaction, transaction->id) ||
	    inputs_sign(transaction, sender, all_unspent) == -1)
	{
		tx_free(transaction);
		return (NULL);
	}

	return (transaction);
}
