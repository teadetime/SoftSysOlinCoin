#pragma once

#include <uthash.h>

#define COMMAND_NAME_LEN 128

typedef struct {
  char *name;
  int (*func)(size_t, char **);
  size_t num_args;
  UT_hash_handle hh;
} Command;

size_t arg_len(char **args);
int shell_mine(size_t num_args, char **args);
int shell_build_tx(size_t num_args, char **args);
int shell_print_chain(size_t num_args, char **args);
int shell_print_block(size_t num_args, char **args);
int shell_exit(size_t num_args, char **args);

char *shell_read_line();
char **shell_tokenize(char *line);
int shell_execute(size_t num_args, char **args);
void shell_loop();

void shell_init_globals();
void shell_init_commands();
void shell_init();

extern char *shell_command_names[];
extern int (*shell_command_funcs[]) (size_t, char **);
extern size_t shell_command_num_args[];
Command *shell_commands;
