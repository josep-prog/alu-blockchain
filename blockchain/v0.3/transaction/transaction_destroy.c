#include "transaction.h"

/**
 * transaction_destroy - Deallocates a transaction structure
 *
 * @transaction: Pointer to the transaction to delete
 */
void transaction_destroy(transaction_t *transaction)
{
	if (!transaction)
		return;

	if (transaction->inputs)
		llist_destroy(transaction->inputs, 1, free);
	if (transaction->outputs)
		llist_destroy(transaction->outputs, 1, free);
	free(transaction);
}
