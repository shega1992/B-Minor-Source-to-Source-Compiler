#ifndef PARSER
#define PARSER

#include <stdlib.h>
#include <string.h>

// DECLARATIONS
struct decl 
{
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
};

// STATEMENTS

typedef enum 
{
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK
}stmt_t;

struct stmt
{
	stmt_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
	struct decl* parent_function;	
};


//EXPRESSIONS
typedef enum 
{
	EXPR_ASSIGN,
	EXPR_OR,
	EXPR_AND,
	EXPR_NEQ,
	EXPR_EQ,
	EXPR_GTE,
	EXPR_GT,
	EXPR_LTE,
	EXPR_LT,
	EXPR_SUB,
	EXPR_ADD,
	EXPR_MOD,
	EXPR_DIV,
	EXPR_MUL,
	EXPR_EXP,
	EXPR_NOT,
	EXPR_NEG,
	EXPR_DEC,
	EXPR_INC,
	EXPR_GROUP,
	EXPR_CALL,
	EXPR_SUBSCRIPT,

	EXPR_NAME,
	EXPR_INTEGER_LITERAL,
	EXPR_BOOLEAN_LITERAL,
	EXPR_CHAR_LITERAL,
	EXPR_STRING_LITERAL
}expr_t;

struct expr
{
	expr_t kind;
	struct expr *left;
	struct expr *right;

	const char *name;
	int integer_value;
	int boolean_value;
	char character_literal;
	const char *string_literal;
	struct symbol *symbol;
	struct expr* next;
};


// TYPES
typedef enum
{
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHARACTER,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION
}type_t;

struct type
{
	type_t kind;
	struct type *subtype;
	struct param_list *params;
	int size;
};

struct param_list
{
	char *name;
	struct type *type;
	struct symbol *symbol;
	struct param_list *next;
};

// FUNCTION PROTOTYPE
struct decl * decl_create( char *name,
						   struct type *type,
                           struct expr *value,
                           struct stmt *code,
                           struct decl *next );

struct stmt * stmt_create( stmt_t kind,
    struct decl *decl, struct expr *init_expr,
    struct expr *expr, struct expr *next_expr,
    struct stmt *body, struct stmt *else_body,
    struct stmt *next );

struct expr * expr_create( expr_t kind,
                           struct expr *L, struct expr *R );

struct expr * expr_create_name(const char *name);
struct expr * expr_create_integer_literal(int i);
struct expr * expr_create_boolean_literal(int b);
struct expr * expr_create_char_literal(char c);
struct expr * expr_create_string_literal(const char * str);

struct type * type_create(type_t kind,
	                      struct type *subtype,
                          struct param_list *params,
                          int size);

struct param_list * param_list_create(char *name,
									 struct type *type,
	                                 struct param_list *next);




#endif