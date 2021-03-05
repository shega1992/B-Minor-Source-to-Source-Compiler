#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H
#include "semantic_analysis.h"

// type helper function prototypes
struct type* type_copy(struct type* t);
int type_compare(struct type* a, struct type* b);
void type_delete(struct type *t);
void type_print( struct type *t );

// parameter list helper function prototypes
struct param_list* param_list_copy(struct param_list* p);
int param_list_compare_type(struct param_list* a, struct param_list* b);
int param_list_compare_decl(struct param_list* p1, struct param_list* p2);
int param_list_compare_call(struct param_list* p, struct expr* e);
void param_list_delete(struct param_list *a);
void param_list_print( struct param_list *a );

// statement helper function prototypes
void stmt_return_typecheck(struct decl* d);
void stmt_return_assign(struct stmt* s, struct decl* d);

// expression helper function prototypes
void expr_print( struct expr *e);
void exprs_print(struct expr* e, int indent);
int expr_check_precedence(struct expr* left, struct expr* right);
int expr_priority(struct expr* e);

#endif