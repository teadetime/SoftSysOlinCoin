#pragma once

#include <uthash.h>

#define COMMAND_NAME_LEN 128

typedef struct {
  char *name;
  int (*func)(char **);
  size_t num_args;
  char *help;
} Command;

typedef struct {
  Command *cmd;
  UT_hash_handle hh;
} CommandPool;

size_t arg_len(char **args);
void str_to_buf(
    unsigned char *dest, char *hex_str,
    size_t dest_len, size_t src_len
);

int shell_mine(char **args);
int shell_build_tx(char **args);
int shell_print_chain(char **args);
int shell_print_block(char **args);
int shell_exit(char **args);
int shell_help(char **args);

char *shell_read_line();
char **shell_tokenize(char *line);
int shell_execute(size_t num_args, char **args);
void shell_loop();

void shell_init_globals();
void shell_init_commands();
void shell_init();

CommandPool *shell_command_pool;
