#pragma once

typedef struct {
  int (**setup)(void**);
  int (**teardown)(void**);
} ComposedFixture;

int fixture_setup_compose(void **state);
int fixture_teardown_compose(void **state);

int fixture_clear_utxo_pool(void **state);
int fixture_setup_utxo_pool(void **state);
int fixture_teardown_utxo_pool(void **state);

int fixture_clear_wallet(void **state);
int fixture_setup_wallet(void **state);
int fixture_teardown_wallet(void **state);
