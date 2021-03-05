#include "ast.h"
#include "helper_functions.h"

struct decl * decl_create( char *name,
						   struct type *type,
                           struct expr *value,
                           struct stmt *code,
                           struct decl *next )
{
	struct decl *d = malloc(sizeof(*d));
	d->name = name;
	d->type = type;
	d->value = value;
	d->code = code;
	d->next = next;
	return d;
}

struct stmt * stmt_create( stmt_t kind,
    struct decl *decl, struct expr *init_expr,
    struct expr *expr, struct expr *next_expr,
    struct stmt *body, struct stmt *else_body,
    struct stmt *next )
{
	struct stmt *s = malloc(sizeof(*s));
	s->kind = kind;
	s->decl = decl;
	s->init_expr = init_expr;
	s->expr = expr;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->next = next;
	return s;
}

struct expr * expr_create( expr_t kind,
                           struct expr *L, struct expr *R )
{
	struct expr *e = malloc(sizeof(*e));
	e->kind = kind;
	e->left = L;
	e->right = R;
	e->next = NULL;
	return e;
}

struct expr * expr_create_name(const char *name)
{
	struct expr *e = expr_create(EXPR_NAME, 0, 0);
	e->name = name;

	return e;
}

struct expr * expr_create_integer_literal(int i)
{
	struct expr *e =  expr_create(EXPR_INTEGER_LITERAL, 0, 0);
	e->integer_value = i;

	return e;
}

struct expr * expr_create_boolean_literal(int b)
{
	struct expr *e = expr_create(EXPR_BOOLEAN_LITERAL, 0, 0);
	e->boolean_value = b;

	return e;
}

struct expr * expr_create_char_literal(char c)
{
	struct expr *e = expr_create(EXPR_CHAR_LITERAL, 0, 0);
	e->character_literal = c;

	return e;
}

struct expr * expr_create_string_literal(const char * str)
{
	struct expr *e = expr_create(EXPR_STRING_LITERAL, 0, 0);
	e->string_literal = str;

	return e;
}

struct type * type_create(type_t kind,
	                      struct type *subtype,
                          struct param_list *params,
                          int size)
{
	struct type *t = malloc(sizeof(*t));
	t->kind = kind;
	t->subtype = subtype;
	t->params = params;
	t->size = size;
	return t;
}

struct param_list * param_list_create(char *name,
									 struct type *type,
	                                 struct param_list *next)
{
	struct param_list *p = malloc(sizeof(*p));
	p->name = strdup(name);
	p->type = type_copy(type);
	p->next = next;
	return p;
}


