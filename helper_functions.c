#include <stdio.h>
#include "helper_functions.h"

extern int typerr;

// type helper functions
struct type* type_copy(struct type* t) {
    if (!t) return 0;
    return type_create(t->kind, type_copy(t->subtype), param_list_copy(t->params), t->size);
}

int type_compare(struct type* a, struct type* b) {
    if (a->kind == b->kind) {
        return 1;
    } else if (a->kind == TYPE_ARRAY && b->kind == TYPE_ARRAY) {
        if (type_compare(a->subtype, b->subtype)) return 1;
    } else if (a->kind == TYPE_FUNCTION && b->kind == TYPE_FUNCTION) {
        if(type_compare(a->subtype, b->subtype) && param_list_compare_type(a->params, b->params)) return 1;
    } else {
        return 0; // there's a problem
    }
}

void type_delete(struct type* t) {
    if (!t) return;

    type_delete(t->subtype);
    if (t->kind == TYPE_FUNCTION) {
        param_list_delete(t->params);
    }

    if (t) free(t);
}

void type_print(struct type* t) {
    if (!t) return;
    switch(t->kind) {
        case TYPE_VOID:
            printf("void");
            break;
        case TYPE_BOOLEAN:
            printf("boolean");
            break;
        case TYPE_CHARACTER:
            printf("char");
            break;
        case TYPE_INTEGER:
            printf("integer");
            break;
        case TYPE_STRING:
            printf("string");
            break;
        case TYPE_FUNCTION:
            printf("function ");
            if (t->subtype) {type_print(t->subtype);}
            printf("(");
            param_list_print(t->params);
            printf(")");
            break;
        case TYPE_ARRAY:
            printf("array [");
            if (t->size) printf("%d", t->size);
            printf("] ");
            type_print(t->subtype);
            break;
    }
    return;
}

// parameter list helper functions
struct param_list* param_list_copy(struct param_list* p) {
    if (!p) return 0;

    struct param_list* pt = param_list_create(p->name, p->type, 0);
    pt->next = param_list_copy(p->next);
    return pt;
}

int param_list_compare_type(struct param_list* a, struct param_list* b) { // iterative compare
    struct param_list* pptra = a;
    struct param_list* pptrb = b;

    while (pptra->next && pptrb->next) {

        if (!type_compare(pptra->type, pptrb->type)) {
            return 0;
        }

        if(pptra->next) {pptra = pptra->next;}
        if(pptrb->next) {pptrb = pptrb->next;}
        if ((pptra->next && !pptrb->next) || (!pptra->next && pptrb->next)) return 0;
    }

    // all is good, return 1
    printf("param_list_compare_p: param list matches\n");
    return 1;
}

int param_list_compare_decl(struct param_list* p1, struct param_list* p2) { // recursive compare, doesn't work
    if (!type_compare(p1->type, p2->type)) {
        return 0;
    } else {
        return 1;
    }

    param_list_compare_decl(p1->next, p2->next);
}

int param_list_compare_call(struct param_list* p, struct expr* e) { // recursive compare, doesn't work
    if (!type_compare(p->type, expr_typecheck(e))) {
        return 0;
    } else {
        return 1;
    }

    param_list_compare_call(p->next, e->next);
}

void param_list_delete(struct param_list* p) {
    if (!p) return;

    type_delete(p->type);
    if (p->name) free(p->name);
    param_list_delete(p->next);

    if (p) free(p);
}

void param_list_print(struct param_list *p) {
    if (!p) return;
    printf("%s :", p->name);
    type_print(p->type);
    if (p->next) printf(", ");
    param_list_print(p->next);
}

// statement helper functions
void stmt_return_typecheck(struct decl *d)
{ // looks for all return statements within function code
    // d is the function declaration, code is the code inside
    struct stmt* ptr = d->code;

    while (ptr)
    {
        stmt_return_assign(ptr, d); // assigning the

        if (ptr->kind == STMT_RETURN)
        { // compare return statement type and function type
            ptr->parent_function = d;
            if (d->type->subtype->kind == TYPE_VOID) {
                if (ptr->expr->kind) {
                    fprintf(stderr, "\ntype error: void function %s must have void return statement\n", d->name);
                    typerr++;
                }
                ptr->expr->kind = TYPE_VOID;
            } else if (!type_compare(d->type->subtype, expr_typecheck(ptr->expr))) {
                fprintf(stderr, "type error: type mismatch between function %s and return value\n", d->name);
                typerr++;
            }
        }

        if (ptr->next)
        {
            ptr = ptr->next;
        }
        else
        {
            ptr = 0;
        }
    }
}

void stmt_return_assign(struct stmt* s, struct decl* d) {
    if (!s) return;

    if (s->kind == STMT_RETURN) {
        s->parent_function = d;
    }
    stmt_return_assign(s->body, d);
    stmt_return_assign(s->else_body, d);
    stmt_return_assign(s->next, d);
}

//expression helper functions
void expr_print(struct expr *e)
{
    if (!e)
        return;

    // handle parentheses if they are necessary. Compare to one level up and remove them if they are not needed
    if (e->left)
    {
        if (e->left->kind == EXPR_GROUP)
        {
            if (e->left->right)
            {
                if (!expr_check_precedence(e->left->right, e))
                {
                    e->left = e->left->right;
                }
            }
        }
    }

    if (e->right)
    {
        if (e->right->kind == EXPR_GROUP)
        {
            if (e->right->right)
            {
                if (!expr_check_precedence(e->right->right, e))
                {
                    e->right = e->right->right;
                }
            }
        }
    }

    switch (e->kind)
    {
    case EXPR_ASSIGN:
        expr_print(e->left);
        printf("=");
        expr_print(e->right);
        break;
    case EXPR_OR:
        expr_print(e->left);
        printf("||");
        expr_print(e->right);
        break;
    case EXPR_AND:
        expr_print(e->left);
        printf("&&");
        expr_print(e->right);
        break;
    case EXPR_GT:
        expr_print(e->left);
        printf(">");
        expr_print(e->right);
        break;
    case EXPR_GTE:
        expr_print(e->left);
        printf(">=");
        expr_print(e->right);
        break;
    case EXPR_LT:
        expr_print(e->left);
        printf("<");
        expr_print(e->right);
        break;
    case EXPR_LTE:
        expr_print(e->left);
        printf("<=");
        expr_print(e->right);
        break;
    case EXPR_EQ:
        expr_print(e->left);
        printf("==");
        expr_print(e->right);
        break;
    case EXPR_NEQ:
        expr_print(e->left);
        printf("!=");
        expr_print(e->right);
        break;
    case EXPR_ADD:
        expr_print(e->left);
        printf("+");
        expr_print(e->right);
        break;
    case EXPR_SUB:
        expr_print(e->left);
        printf("-");
        expr_print(e->right);
        break;
    case EXPR_MUL:
        expr_print(e->left);
        printf("*");
        expr_print(e->right);
        break;
    case EXPR_DIV:
        expr_print(e->left);
        printf("/");
        expr_print(e->right);
        break;
    case EXPR_MOD:
        expr_print(e->left);
        printf("%%");
        expr_print(e->right);
        break;
    case EXPR_EXP:
        expr_print(e->left);
        printf("^");
        expr_print(e->right);
        break;
    case EXPR_NOT:
        printf("!");
        expr_print(e->right);
        break;
    case EXPR_NEG:
        printf("-");
        expr_print(e->right);
        break;
    case EXPR_INC:
        expr_print(e->left);
        printf("++");
        break;
    case EXPR_DEC:
        expr_print(e->left);
        printf("--");
        break;
    case EXPR_INTEGER_LITERAL:
        printf("%d", e->integer_value);
        break;
    case EXPR_BOOLEAN_LITERAL:
        switch (e->boolean_value)
        {
        case 0:
            printf("false");
            break;
        case 1:
            printf("true");
            break;
        }
        break;
    case EXPR_STRING_LITERAL:
        printf("%s", e->string_literal);
        break;
    case EXPR_CHAR_LITERAL:
        printf("'");
        switch(e->character_literal)
        {
            case 0: printf("\\0"); break;
            case 9: printf("\\t"); break;
            case 10: printf("\\n"); break;
            case 13: printf("\\r"); break;
            case 39: printf("\\'"); break;
            case 92: printf("\\\\"); break;
            default: printf("%c", e->character_literal); break;
        }
        printf("'");
        break;
    case EXPR_NAME:
        printf("%s", e->name);
        break;
    case EXPR_CALL:
        expr_print(e->left);
        printf("(");
        exprs_print(e->right, 0);
        printf(")");
        break;
    case EXPR_GROUP:
        printf("(");
        expr_print(e->right);
        printf(")");
        break;
    case EXPR_SUBSCRIPT:
        expr_print(e->left);
        printf("[");
        expr_print(e->right);
        printf("]");

        // More array accesses for multiple dimensions
        struct expr *ee = e->right;
        while (ee->right)
        {
            printf("[");
            expr_print(ee);
            printf("]");
            ee = ee->right;
        }
        break;
    }
}

void exprs_print(struct expr *e, int indent)
{
    if (!e)
        return;
    expr_print(e);
    if (e->next)
    {
        printf(", ");
    }
    exprs_print(e->next, indent);
}

int expr_check_precedence(struct expr *left, struct expr *right)
{
    if (expr_priority(left) > expr_priority(right))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int expr_priority(struct expr *e)
{
    switch (e->kind)
    {
    case EXPR_GROUP:
    case EXPR_SUBSCRIPT:
    case EXPR_CALL:
        return 1;
        break;
    case EXPR_INC:
    case EXPR_DEC:
        return 2;
        break;
    case EXPR_NEG:
    case EXPR_NOT:
        return 3;
        break;
    case EXPR_EXP:
        return 4;
        break;
    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
        return 5;
        break;
    case EXPR_ADD:
    case EXPR_SUB:
        return 6;
        break;
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_EQ:
    case EXPR_NEQ:
        return 7;
        break;
    case EXPR_AND:
        return 8;
        break;
    case EXPR_OR:
        return 9;
        break;
    case EXPR_ASSIGN:
        return 10;
        break;
    }
    return -1;
}
