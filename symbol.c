#include "symbol.h"
#include "helper_functions.h"
#include <string.h>

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name ) 
{
    struct symbol* s = malloc(sizeof(*s));

    s->kind = kind;
    s->type = type_copy(type);
    s->name = strdup(name);

    return s;
}

struct symbol* symbol_copy(struct symbol* in) 
{
    struct symbol* s = symbol_create(in->kind, in->type, in->name);
    s->which = in->which;

    return s;
}



