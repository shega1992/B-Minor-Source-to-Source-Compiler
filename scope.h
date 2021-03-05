#ifndef SCOPE_H
#define SCOPE_H
#include "symbol.h" // also includes ast.h
#include "hash_table.h"
#include "helper_functions.h"

struct scope_stack {
    struct hash_table* elem;
    struct scope_stack* next;
    symbol_t scp;
    int location;
};

void scope_enter();
void scope_function_enter();
void scope_exit();
int scope_level();

void scope_bind(const char* name, struct symbol* s);
struct symbol * scope_lookup( const char *name );
struct symbol * scope_lookup_current( const char *name);

#endif

