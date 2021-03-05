#ifndef TRANSLATE_H_INCLUDED
#define TRANSLATE_H_INCLUDED

#include "semantic_analysis.h"

void translate_into_CPP(struct decl *d);
void decl_translate(struct decl *d, long indent);
void stmt_translate(struct stmt *s, long indent);
void expr_translate(struct expr *e);
void exprs_translate(struct expr *e, long indent);
void type_translate(struct type *t);
void param_list_translate(struct param_list *p);
void print_tabs(long indent);

FILE * fp;

#endif
