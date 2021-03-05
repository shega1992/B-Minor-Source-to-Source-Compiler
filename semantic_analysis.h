#ifndef SEM_ANALYSIS
#define  SEM_ANALYSIS
#include "scope.h"

// resolve function prototypes
void decl_resolve(struct decl* d, int print);
void expr_resolve(struct expr* e);
void exprs_resolve(struct expr* e);
void param_list_resolve(struct param_list* a, int print);
void stmt_resolve(struct stmt* s, int print);

// typecheck function prototypes
void decl_typecheck(struct decl* d);
struct type *expr_typecheck(struct expr *e);
void stmt_typecheck(struct stmt *s);

#endif


