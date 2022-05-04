#include "shell.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "constants.h"
#include "init_globals.h"
#include "mempool.h"
#include "blockchain.h"
#include "runtime.h"
#include "wallet.h"
#include "create_block.h"
#include "handle_block.h"

#include "crypto.h"

#define DELIMS " \t\n"
#define ARG_SIZE 32

Command shell_commands[] = {
  // {
  //   .name = "mine", .func = &shell_mine, .num_args = 1,
  //   .help = "Usage: mine\nMine a new block"
  // },
  {
    .name = "create_tx", .func = &shell_build_tx, .num_args = 4,
    .help = "Usage: create_tx ADDRESS AMT FEE\nCreate a new transaction"
  },
  {
    .name = "print_chain", .func = &shell_print_chain, .num_args = 1,
    .help = "Usage: print_chain\nPrint the blockchain"
  },
  {
    .name = "print_block", .func = &shell_print_block, .num_args = 2,
    .help = "Usage: print_block BLOCK_HASH\nPrint a block"
  },
  {
    .name = "exit", .func = &shell_exit, .num_args = 1,
    .help = "Usage: exit\nExit shell"
  },
  {
    .name = "help", .func = &shell_help, .num_args = 1,
    .help = "Usage: help\nShow help"
  },
};

size_t arg_len(char **args) {
  size_t l;

  if (!args)
    return 0;

  // NOTE: Relies on args being null terminated, as it is in shell_tokenize()
  l = 0;
  while (args[l++]);
  return l - 1;
}

/**
 * @brief Converts a string of hex values to a buffer
 *
 * Fills the destination with 0s and terminates early if either input is shorter
 * than the other. This means that a string of hex values less than 64
 * characters long produces a buffer with a bunch of 0s on the end.
 *
 * @param dest buffer to write to
 * @param hex_str hex string to read from
 * @param dest_len length of destination buffer
 * @param src_len length of source string
 */
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

// int shell_mine(Globals *globals, char **args) {
//   (void)args;
//   Block *block;
//   unsigned char header_hash[BLOCK_HASH_LEN];

//   printf("Mining...\n");
//   block = mine_block();
//   handle_new_block(block);

//   // This might not be needed anymore...
//   hash_blockheader(header_hash, &block->header);
//   printf("Block mined!\n\n");
//   dump_buf("", "Hash: ", header_hash, BLOCK_HASH_LEN);
//   pretty_print_block(block, "");

//   unsigned char tx_hash[TX_HASH_LEN];
//   hash_tx(tx_hash, block->txs[0]);
//   return 0;
// }

int shell_build_tx(Globals *globals, char **args) {
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

  pthread_mutex_lock(&globals->wallet_pool_lock);
  pthread_mutex_lock(&globals->key_pool_lock);

  tx = build_tx(options);
  queue_add_void(globals->queue_tx, tx);
  pthread_mutex_unlock(&globals->wallet_pool_lock);
  pthread_mutex_unlock(&globals->key_pool_lock);
  printf("Transaction added to buffer!\n\n");
  pretty_print_tx(tx, "");

  free(output);
  free(options);

  return 0;
}

int shell_print_chain(Globals *globals, char **args) {
  (void)args;
  pthread_mutex_lock(&globals->blockchain_lock);
  pretty_print_blockchain_hashmap();
  pthread_mutex_unlock(&globals->blockchain_lock);
  return 0;
}

int shell_print_block(Globals *globals, char **args) {
  unsigned char buf[BLOCK_HASH_LEN];
  Block *block;

  str_to_buf(
      buf, args[1],
      BLOCK_HASH_LEN, strlen(args[1])
  );
  pthread_mutex_lock(&globals->blockchain_lock);
  int ret_find = blockchain_find_leveldb(&block, buf);
  pthread_mutex_unlock(&globals->blockchain_lock);
  if (!block || ret_find != 0) {
    printf("%s: block not found\n", args[0]);
    return 0;
  }
  pretty_print_block(block, "");
  if(block != NULL){
    free(block); //TODO: USE PROPER BLOCK TEARDOWN
  }
  return 0;
}

int shell_exit(Globals *globals, char **args) {
  pthread_mutex_lock(&globals->utxo_pool_lock);
  pthread_mutex_lock(&globals->blockchain_lock);
  pthread_mutex_lock(&globals->utxo_to_tx_lock);
  pthread_mutex_lock(&globals->wallet_pool_lock);
  pthread_mutex_lock(&globals->key_pool_lock);
  pthread_mutex_lock(&globals->mempool_lock);
  (void)args;
  return 1;
}

int shell_help(Globals *globals, char **args) {
  int len;
  (void)args;
  (void)globals;

  len = sizeof(shell_commands) / sizeof(Command);
  printf("Available shell commands:\n");
  printf(LINE_BREAK);
  for (int i = 0; i < len; i++) {
    printf("%s\n", shell_commands[i].name);
    printf("%s\n", shell_commands[i].help);
    printf(SOFT_LINE_BREAK);
  }
  return 0;
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

int shell_execute(Globals *globals, size_t num_args, char **args) {
  CommandPool *entry;
  Command *cmd;
  int len, i;

  HASH_FIND_STR(shell_command_pool, args[0], entry);

  if (!entry) {
    printf("command not found: %s\n", args[0]);
    printf("available commands: ");
    len = sizeof(shell_commands) / sizeof(Command);
    for (i = 0; i < len; i++)
      printf("%s, ", shell_commands[i].name);
    printf("\n");
    return 0;
  }

  cmd = entry->cmd;
  if (num_args < cmd->num_args) {
    printf("%s: missing arguments\n", cmd->name);
    return 0;
  }
  else if (num_args > cmd->num_args) {
    printf("%s: too many arguments\n", cmd->name);
    return 0;
  }

  return cmd->func(globals, args);
}

void shell_loop(Globals *globals) {
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
    status = shell_execute(globals, num_args, args);

    free(line);
    free(args);
  }
}

void shell_init_globals() {
  node_init(PROD_DB_LOC);
  miner_init();
  wallet_init_leveldb(PROD_DB_LOC);
}

void shell_init_commands() {
  int i, len;
  CommandPool *entry;

  shell_command_pool = NULL;
  len = sizeof(shell_commands) / sizeof(Command);
  for (i = 0; i < len; i++) {
    entry = malloc(sizeof(CommandPool));
    entry->cmd = &shell_commands[i];
    HASH_ADD_KEYPTR(
      hh, shell_command_pool,
      entry->cmd->name, strlen(entry->cmd->name),
      entry
    );
  }
}

void shell_init() {
  //shell_init_globals();
  shell_init_commands();
}

// int main() {
//   // shell_init();
//   // shell_loop();
//   return 1;
// }
