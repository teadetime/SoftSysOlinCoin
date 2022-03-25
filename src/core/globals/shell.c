#include "shell.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "create_block.h"
#include "constants.h"
#include "wallet.h"
#include "blockchain.h"
#include "mempool.h"

#define DELIMS " \t\n"
#define ARG_SIZE 32

char *shell_command_names[] = {
  "mine",
  "create_tx",
  "print_chain",
  "print_block",
  "exit",
};

int (*shell_command_funcs[]) (size_t, char **) = {
  &shell_mine,
  &shell_build_tx,
  &shell_print_chain,
  &shell_print_block,
  &shell_exit,
};

size_t shell_command_num_args[] = {
  1,
  4,
  1,
  2,
  1,
};

size_t arg_len(char **args) {
  size_t l;

  if (!args)
    return 0;

  l = 0;
  while (args[l++]);
  return l - 1;
}

void str_to_buf(
    unsigned char *dest, char *hex_str,
    size_t dest_len, size_t src_len
) {
  size_t i;

  i = 0;
  memset(dest, 0, dest_len);
  while (i < dest_len && i * 2 < src_len) {
    sscanf(hex_str + 2 * i, "%02x", (unsigned int*)&dest[i]);
    i++;
  }
}

int shell_mine(size_t num_args, char **args) {
  (void)args;
  (void)num_args;
  Block *block;
  unsigned char header_hash[BLOCK_HASH_LEN];

  printf("Mining...\n");
  block = mine_block();
  hash_blockheader(header_hash, &block->header);
  printf("Block mined!\n\n");
  dump_buf("", "Hash: ", header_hash, BLOCK_HASH_LEN);
  pretty_print_block(block, "");

  unsigned char tx_hash[TX_HASH_LEN];
  hash_tx(tx_hash, block->txs[0]);
  return 0;
}

int shell_build_tx(size_t num_args, char **args) {
  (void)num_args;
  Transaction *tx;
  TxOptions *options;
  Output *output;
  char *end;

  output = malloc(sizeof(Output));
  str_to_buf(
      output->public_key_hash, args[1],
      PUB_KEY_HASH_LEN, strlen(args[1])
  );
  output->amt = strtoul(args[2], &end, 10);

  options = malloc(sizeof(TxOptions));
  options->num_dests = 1;
  options->dests = output;
  options->tx_fee = strtoul(args[3], &end, 10);

  tx = build_tx(options);
  mempool_add(tx);

  printf("Transaction created!\n\n");
  pretty_print_tx(tx, "");

  free(output);
  free(options);

  return 0;
}

int shell_print_chain(size_t num_args, char **args) {
  (void)args;
  (void)num_args;
  pretty_print_blockchain_hashmap();
  return 0;
}

int shell_print_block(size_t num_args, char **args) {
  (void)num_args;
  unsigned char buf[BLOCK_HASH_LEN];
  Block *block;

  str_to_buf(
      buf, args[1],
      BLOCK_HASH_LEN, strlen(args[1])
  );
  block = blockchain_find(buf);

  if (!block) {
    printf("print_block: block not found\n");
    return 0;
  }
  pretty_print_block(block, "");
  return 0;
}

int shell_exit(size_t num_args, char **args) {
  (void)args;
  (void)num_args;
  exit(EXIT_SUCCESS);
}

char *shell_read_line() {
  char *line;
  size_t len;

  line = NULL;
  if (getline(&line, &len, stdin) == -1) {
    if (feof(stdin))
      exit(EXIT_SUCCESS);
    else {
      printf("Error parsing line.\n");
      exit(EXIT_FAILURE);
    }
  }
  // Get rid of newline at end
  line[strlen(line) - 1] = '\0';

  len = strlen(line);
  if (len == 0) {
    free(line);
    return NULL;
  }
  else
    return line;
}

char **shell_tokenize(char *line) {
  char **args;
  char *arg;
  int i, args_len;

  args_len = ARG_SIZE;
  args = malloc(sizeof(char*) * args_len);
  arg = strtok(line, DELIMS);
  i = 0;
  while (arg) {
    args[i++] = arg;
    arg = strtok(NULL, DELIMS);
    if (i >= args_len) {
      args_len *= 2;
      args = realloc(args, args_len);
    }
  }
  /* args = realloc(args, i + 1); */ // Weird compiler problems here
  args[i] = NULL;

  return args;
}

int shell_execute(size_t num_args, char **args) {
  Command *cmd;

  HASH_FIND_STR(shell_commands, args[0], cmd);

  if (!cmd) {
    printf("command not found: %s\n", args[0]);
    return 0;
  }
  else if (num_args < cmd->num_args) {
    printf("%s: missing arguments\n", cmd->name);
    return 0;
  }
  else if (num_args > cmd->num_args) {
    printf("%s: too many arguments\n", cmd->name);
    return 0;
  }

  return cmd->func(num_args, args);
}

void shell_loop() {
  char *line;
  char **args;
  int status;
  size_t num_args;

  status = 0;
  while (status == 0) {
    fprintf(stdout, "> ");

    line = shell_read_line();
    if (!line)
      continue;
    args = shell_tokenize(line);
    num_args = arg_len(args);
    status = shell_execute(num_args, args);

    free(line);
    free(args);
  }
}

void shell_init_globals() {
  node_init();
  miner_init();
  wallet_init();
}

void shell_init_commands() {
  int i, len;
  Command *cmd;

  shell_commands = NULL;
  len = sizeof(shell_command_names) / sizeof(char*);
  for (i = 0; i < len; i++) {
    cmd = malloc(sizeof(Command));
    cmd->name = shell_command_names[i];
    cmd->func = shell_command_funcs[i];
    cmd->num_args = shell_command_num_args[i];
    HASH_ADD_KEYPTR(hh, shell_commands, cmd->name, strlen(cmd->name), cmd);
  }
}

void shell_init() {
  shell_init_globals();
  shell_init_commands();
}

int main() {
  shell_init();
  shell_loop();
  return 1;
}
