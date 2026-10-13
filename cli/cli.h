#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <llist.h>

#include "../crypto/hblk_crypto.h"
#include "../blockchain/v0.3/blockchain.h"

#define PROMPT "hblk> "
#define MAX_ARGS 8
#define PUB_HEX_LEN (EC_PUB_LEN * 2)

/**
 * struct state_s - State of the CLI session
 *
 * @wallet:     EC key pair of the user
 * @blockchain: Local Blockchain
 * @tx_pool:    Local transaction pool (list of pending `transaction_t *`)
 */
typedef struct state_s
{
	EC_KEY          *wallet;
	blockchain_t    *blockchain;
	llist_t         *tx_pool;
} state_t;

/**
 * struct command_s - Command of the CLI
 *
 * @name:  Name of the command
 * @usage: Arguments expected by the command
 * @help:  Short description of the command
 * @func:  Function executing the command
 */
typedef struct command_s
{
	char const  *name;
	char const  *usage;
	char const  *help;
	int         (*func)(state_t *state, int argc, char **argv);
} command_t;

/* Commands: return 0 upon success, 1 upon failure, -1 to exit the CLI */
int cmd_wallet_load(state_t *state, int argc, char **argv);
int cmd_wallet_save(state_t *state, int argc, char **argv);
int cmd_send(state_t *state, int argc, char **argv);
int cmd_mine(state_t *state, int argc, char **argv);
int cmd_info(state_t *state, int argc, char **argv);
int cmd_load(state_t *state, int argc, char **argv);
int cmd_save(state_t *state, int argc, char **argv);

/* Utils */
llist_t *unspent_available(llist_t *all_unspent, llist_t *transactions);
uint64_t unspent_balance(llist_t *unspent, uint8_t const pub[EC_PUB_LEN]);
void print_hex(uint8_t const *buf, size_t len);

#endif /* _CLI_H_ */
