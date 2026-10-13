#include "cli.h"

static int cmd_help(state_t *state, int argc, char **argv);
static int cmd_exit(state_t *state, int argc, char **argv);

static command_t const commands[] = {
	{"wallet_load", "<path>", "Load wallet (EC key pair)", cmd_wallet_load},
	{"wallet_save", "<path>", "Save wallet (EC key pair)", cmd_wallet_save},
	{"send", "<amount> <address>", "Send coins", cmd_send},
	{"mine", "", "Mine a block", cmd_mine},
	{"info", "", "Display information about the Blockchain", cmd_info},
	{"load", "<path>", "Load a Blockchain from a file", cmd_load},
	{"save", "<path>", "Save the Blockchain into a file", cmd_save},
	{"help", "", "Display this list of commands", cmd_help},
	{"exit", "", "Exit the CLI", cmd_exit},
	{NULL, NULL, NULL, NULL}
};

/**
 * cmd_help - Displays the list of commands
 *
 * @state: State of the CLI session (unused)
 * @argc:  Number of arguments (unused)
 * @argv:  Arguments (unused)
 *
 * Return: Always 0
 */
static int cmd_help(state_t *state, int argc, char **argv)
{
	command_t const *cmd;

	(void)state;
	(void)argc;
	(void)argv;
	for (cmd = commands; cmd->name; cmd++)
		printf("  %-11s %-19s %s\n", cmd->name, cmd->usage, cmd->help);

	return (0);
}

/**
 * cmd_exit - Exits the CLI
 *
 * @state: State of the CLI session (unused)
 * @argc:  Number of arguments (unused)
 * @argv:  Arguments (unused)
 *
 * Return: Always -1
 */
static int cmd_exit(state_t *state, int argc, char **argv)
{
	(void)state;
	(void)argc;
	(void)argv;

	return (-1);
}

/**
 * execute - Parses a line and executes the matching command
 *
 * @state: State of the CLI session
 * @line:  Line typed by the user
 *
 * Return: -1 if the CLI must exit, the status of the command otherwise
 */
static int execute(state_t *state, char *line)
{
	char *argv[MAX_ARGS + 1];
	command_t const *cmd;
	int argc = 0;

	argv[0] = strtok(line, " \t\r\n");
	while (argv[argc] && argc < MAX_ARGS)
		argv[++argc] = strtok(NULL, " \t\r\n");
	if (argc == 0)
		return (0);

	for (cmd = commands; cmd->name; cmd++)
	{
		if (strcmp(cmd->name, argv[0]) == 0)
			return (cmd->func(state, argc, argv));
	}

	printf("%s: unknown command (type help)\n", argv[0]);
	return (1);
}

/**
 * state_init - Creates or loads the wallet, and creates the Blockchain
 *
 * @state: State of the CLI session to initialize
 * @path:  Folder to load the wallet from, or NULL to create a new wallet
 *
 * Return: 0 upon success, 1 upon failure
 */
static int state_init(state_t *state, char const *path)
{
	uint8_t pub[EC_PUB_LEN];

	state->wallet = path ? ec_load(path) : NULL;
	if (state->wallet)
		printf("Wallet loaded from %s\n", path);
	else
	{
		if (path)
			printf("Failed to load wallet from %s\n", path);
		state->wallet = ec_create();
		printf("New wallet created\n");
	}
	state->blockchain = blockchain_create();
	state->tx_pool = llist_create(MT_SUPPORT_FALSE);
	if (!state->wallet || !state->blockchain || !state->tx_pool ||
	    !ec_to_pub(state->wallet, pub))
		return (1);

	printf("Address: ");
	print_hex(pub, EC_PUB_LEN);
	printf("\nType help to list the commands\n");

	return (0);
}

/**
 * main - Entry point of the Blockchain CLI
 *
 * @argc: Number of arguments
 * @argv: Arguments: optional path to the folder to load the wallet from
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
	state_t state = {NULL, NULL, NULL};
	char line[1024];
	int status = state_init(&state, argc > 1 ? argv[1] : NULL);

	if (status)
		fprintf(stderr, "Failed to initialize the CLI\n");
	while (status != -1 && state.tx_pool)
	{
		printf(PROMPT);
		fflush(stdout);
		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}
		status = execute(&state, line);
	}

	EC_KEY_free(state.wallet);
	blockchain_destroy(state.blockchain);
	if (state.tx_pool)
		llist_destroy(state.tx_pool, 1, (node_dtor_t)transaction_destroy);

	return (state.tx_pool ? EXIT_SUCCESS : EXIT_FAILURE);
}
