#include <stdio.h>
#include <string.h>
#include "semantic_analysis.h"


extern int reserr;
extern int typerr;

// resolve functions
void decl_resolve(struct decl *d, int print)
{
    if (!d)
        return;

    symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    d->symbol = symbol_create(kind, type_copy(d->type), strdup(d->name));
    d->symbol->which = 0;

    expr_resolve(d->value);
    scope_bind(d->name, d->symbol);

    if (print)
    {
        if (kind == SYMBOL_GLOBAL)
        {
            printf("%s resolves to global %s\n", d->name, d->name);
        }
        else if (kind == SYMBOL_LOCAL)
        {
            printf("%s resolves to local %i\n", d->name, d->symbol->which);
        }
        else if (kind == SYMBOL_PARAM)
        {
            printf("%s resolves to parameter %i\n", d->name, d->symbol->which);
        }
    }

    if (d->type->kind == TYPE_FUNCTION)
    { // enter parameter list scope
        scope_function_enter();
        param_list_resolve(d->type->params, print);
        stmt_resolve(d->code, print);
        scope_exit();
    }

    decl_resolve(d->next, print);
}

void expr_resolve(struct expr *e)
{
    if (!e) return;

    if (e->kind == EXPR_NAME)
    {
        e->symbol = scope_lookup(e->name);
        if (!e->symbol)
        {
            printf("resolution error: %s is undeclared\n", e->name);
            reserr++;
        }
    }
    expr_resolve(e->next);
    expr_resolve(e->left);
    expr_resolve(e->right);
}

void exprs_resolve(struct expr *e)
{
    if (!e)
        return;
    expr_resolve(e);
    exprs_resolve(e->next);
}

void param_list_resolve(struct param_list *p, int print) {
    // enter new variable declaration for each parameter of function
    if (!p) return;

    p->symbol = symbol_create(SYMBOL_PARAM, p->type, p->name);

    scope_bind(p->name, p->symbol);
    if (print) {
        printf("%s resolves to param %i\n", p->name, p->symbol->which);
    }

    param_list_resolve(p->next, print);
}

void stmt_resolve(struct stmt *s, int print)
{
    if (!s)
        return;

    switch (s->kind)
    {
    case STMT_BLOCK:
        scope_enter();
        stmt_resolve(s->body, print);
        scope_exit();
        break;
    case STMT_DECL:
        decl_resolve(s->decl, print);
        break;
    case STMT_EXPR:
        expr_resolve(s->expr);
        break;
    case STMT_FOR:
        scope_enter();
        expr_resolve(s->init_expr);
        expr_resolve(s->expr);
        expr_resolve(s->next_expr);
        stmt_resolve(s->body, print);
        scope_exit();
        break;
    case STMT_IF_ELSE:
        scope_enter();
        expr_resolve(s->expr);
        stmt_resolve(s->body, print);
        if (s->else_body)
        {
            stmt_resolve(s->else_body, print);
        }
        scope_exit();
        break;
    case STMT_PRINT:
        exprs_resolve(s->expr);
        break;
    case STMT_RETURN:
        expr_resolve(s->expr);
        break;
    }
    stmt_resolve(s->next, print);
}

//typecheck functions
void decl_typecheck(struct decl *d)
{
    if (!d)
        return;
    struct type *t = expr_typecheck(d->value);
    //fprintf(stdout, "typechecking decl of %s\n", d->name); fflush(stdout);
    if (d->type->kind == TYPE_FUNCTION)
    { // typecheck the function
        if (d->type->params)
        {
            if (!param_list_compare_decl(d->type->params, d->symbol->type->params))
            {
                fprintf(stderr, "type error: declaration of %s and its prototype have different types\n", d->name);
                typerr++;
            } // compare function definition with resolved name
        }
        stmt_return_typecheck(d);
        //stmt_return_typecheck_recursive(d->code, d);
    }
    else if (d->value)
    {
        if ((d->type->kind != TYPE_ARRAY) && (d->type->kind != TYPE_STRING))
        { // not an array nor string
            if (!type_compare(t, d->symbol->type))
            {
                fprintf(stderr, "type error: declaration of %s and initialization do not match types\n", d->name);
                typerr++;
            }
        }
        else if (d->value && d->type->kind == TYPE_STRING)
        { // this is a string declaration
            if (!type_compare(t, d->symbol->type))
            {
                fprintf(stderr, "type error: string %s is not set to a string literal or string type \n", d->name);
                typerr++;
            }
        }
        else if (d->type->kind == TYPE_ARRAY)
        {                 // array typechecking
            int iter = 0; // to count the numbers and check for size

            struct expr *eptr = d->value;

            while (eptr)
            {
                // compare each type to the individual expression
                iter++;
                if (!type_compare(expr_typecheck(eptr), d->type->subtype))
                { // type_compare(element, array)
                    fprintf(stderr, "type error: array type and item declaration do not match\n");
                    typerr++;
                }

                if (eptr->right)
                {
                    eptr = eptr->right;
                }
                else
                {
                    eptr = 0;
                }
            }
            if (d->type->size)
            {
                if ((iter != d->type->size) && iter)
                { // declared and assigned sizes are different
                    fprintf(stderr, "type error: array %s declaration has %i elements instead of %i\n", d->name, iter, d->type->size);
                    typerr++;
                }
            }
        }

        if (d->type->kind == TYPE_VOID && d->type->kind != TYPE_FUNCTION)
        {
            fprintf(stderr, "type error: cannot declare variable with type void\n");
            typerr++;
        }
    }

    stmt_typecheck(d->code);

    decl_typecheck(d->next);
}

struct type *expr_typecheck(struct expr *e)
{
    if (!e)
        return 0;

    fflush(stdout);

    struct type *lt = expr_typecheck(e->left);
    struct type *rt = expr_typecheck(e->right);

    struct type *result;

    switch (e->kind)
    {
    case EXPR_INTEGER_LITERAL:
        result = type_create(TYPE_INTEGER, 0, 0, 0);
        break;

    case EXPR_STRING_LITERAL:
        result = type_create(TYPE_STRING, 0, 0, 0);
        return result;
        break;

    case EXPR_CHAR_LITERAL:
        result = type_create(TYPE_CHARACTER, 0, 0, 0);
        break;

    case EXPR_BOOLEAN_LITERAL:
        result = type_create(TYPE_BOOLEAN, 0, 0, 0);
        break;

    case EXPR_NAME:
        result = type_copy(e->symbol->type); // copy the type itself
        break;

    case EXPR_GROUP:
        result = type_copy(expr_typecheck(e->right)); // copy the type itself
        break;

    case EXPR_ASSIGN:
        if (lt) {
            if (!type_compare(lt, rt))
            {
                fprintf(stderr, "type error: cannot assign variable %s with an value of different types\n", e->left->name);
                typerr++;
            }
            else if ((lt->kind == TYPE_ARRAY) && (rt->kind == TYPE_ARRAY))
            {
                if (!type_compare(lt->subtype, rt->subtype))
                {
                    fprintf(stderr, "type error: cannot assign two arrays %s and %s of different subtypes\n", e->left->name, e->right->name);
                    typerr++;
                }
            }
            result = type_copy(lt);
        }
        break;

    case EXPR_ADD:
    case EXPR_SUB:
    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
    case EXPR_EXP:
        if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER)
        {
            fprintf(stderr, "type error: cannot do binary operation between a ");
            type_print(rt);
            fprintf(stderr, " (");
            expr_print(e->right);
            fprintf(stderr, ") and a ");
            type_print(lt);
            fprintf(stderr, " (");
            expr_print(e->left);
            fprintf(stderr, ")\n");

            typerr++;
        }
        result = type_create(TYPE_INTEGER, 0, 0, 0);
        break;

    case EXPR_INC:
    case EXPR_DEC:
        if (lt->kind != TYPE_INTEGER)
        {
            fprintf(stderr, "type error: cannot do postfix operation on non-integer ");
            type_print(lt);
            fprintf(stderr, " (");
            expr_print(e->left);
            fprintf(stderr, ")\n");

            typerr++;
        }
        result = type_create(TYPE_INTEGER, 0, 0, 0);
        break;

    case EXPR_NEG:
        if (rt->kind != TYPE_INTEGER)
        {
            fprintf(stderr, "type error: cannot negate non-integer ");
            type_print(rt);
            fprintf(stderr, " (");
            expr_print(e->right);
            fprintf(stderr, ")\n");

            typerr++;
        }
        result = type_create(TYPE_INTEGER, 0, 0, 0);
        break;

    case EXPR_EQ:
    case EXPR_NEQ:
        if (!type_compare(lt, rt))
        {
            fprintf(stderr, "type error: ");
            type_print(lt);
            expr_print(e->left);
            fprintf(stderr, " and ");
            type_print(rt);
            expr_print(e->right);
            fprintf(stderr, "are of different types\n");

            typerr++;
        }
        if (lt->kind == TYPE_VOID || lt->kind == TYPE_ARRAY || lt->kind == TYPE_FUNCTION)
        {
            fprintf(stderr, "type error: cannot use equality operators on ");
            type_print(lt);
            fprintf(stderr, "\n");

            typerr++;
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0);
        break;

    case EXPR_NOT:
        if (rt->kind != TYPE_BOOLEAN)
        {
            fprintf(stderr, "type error: cannot negate non-boolean ");
            type_print(rt);
            fprintf(stderr, " (");
            expr_print(e->right);
            fprintf(stderr, ")\n");

            typerr++;
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0);
        break;

    case EXPR_AND:
    case EXPR_OR:
        if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN)
        {
            if (lt->kind != TYPE_BOOLEAN)
            {
                fprintf(stderr, "type error: cannot perform logical operation on non-boolean\n");

                typerr++;
            }
            if (rt->kind != TYPE_BOOLEAN)
            {
                fprintf(stderr, "type error: cannot perform logical operation on non-boolean\n");

                typerr++;
            }
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0);
        break;

    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_GT:
    case EXPR_GTE:
        if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER)
        {
            if (lt->kind != TYPE_INTEGER)
            {
                fprintf(stderr, "type error: cannot perform logical operation on non-integer ");
                type_print(lt);
                fprintf(stderr, "(");
                expr_print(e->left);
                fprintf(stderr, ")\n");

                typerr++;
            }
            if (rt->kind != TYPE_INTEGER)
            {
                fprintf(stderr, "type error: cannot perform logical operation on non-integer ");
                type_print(rt);
                fprintf(stderr, "(");
                expr_print(e->right);
                fprintf(stderr, ")\n");

                typerr++;
            }
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0);
        break;

    case EXPR_SUBSCRIPT:
        if (lt->kind == TYPE_ARRAY)
        { // array access
            if (rt->kind != TYPE_INTEGER)
            { // non-integer access
                fprintf(stderr, "type error: cannot access %s array with non-integer \n", e->left->name);
                typerr++;
            }
            if (e->right->right)
            { // multi-dimensional mode
                struct expr *rgt = e->right;
                while (rgt)
                {
                    if (expr_typecheck(rgt)->kind != TYPE_INTEGER)
                    {
                        fprintf(stderr, "type error: cannot access %s array with non-integer \n", e->left->name);
                        typerr++;
                    }

                    if (rgt->right)
                    {
                        rgt = rgt->right;
                    }
                    else
                    {
                        rgt = 0;
                    }
                }
            }
            // ADD MULTIPLE
            result = type_copy(lt->subtype);
        }
        else if (lt->kind == TYPE_STRING)
        { // string access
            if (rt->kind != TYPE_INTEGER)
            {
                fprintf(stderr, "type error: cannot index string %s with non-integer\n", e->left->name);
                typerr++;
            }
            result = type_create(TYPE_CHARACTER, 0, 0, 0); // returning a character type;
        }
        else
        {
            fprintf(stderr, "type error: cannot access %s array with non-array \n", e->left->name);
            result = type_copy(lt);

            typerr++;
        }

        break;

    case EXPR_CALL:
        if (lt->kind == TYPE_FUNCTION)
        { // checking the function call
            if (!lt->params) {
                result = type_copy(lt->subtype);
                break;
            }
            if (!param_list_compare_call(e->left->symbol->type->params, e->right)) {
                fprintf(stderr, "type error: parameters not matching in function call of %s\n", e->left->name);
                typerr++;
            }
            result = type_copy(lt->subtype); // return type of function call
        } else {
            fprintf(stderr, "type error: cannot call non-function %s", e->left->name);
            result = type_copy(lt);

            typerr++;
        }
        break;
    }

    type_delete(lt);
    type_delete(rt);

    return result;
}

void stmt_typecheck(struct stmt *s)
{
    if (!s)
        return;

    struct type *t;
    struct type *t1;
    struct type *t2;
    struct type *t3;
    switch (s->kind)
    {
    case STMT_BLOCK:
        stmt_typecheck(s->body);
        break;
    case STMT_DECL:
        decl_typecheck(s->decl);
        break;
    case STMT_EXPR:
        t = expr_typecheck(s->expr);
        type_delete(t);
        break;
    case STMT_PRINT:
        t = expr_typecheck(s->expr);
        if (t->kind == TYPE_FUNCTION || t->kind == TYPE_VOID || t->kind == TYPE_ARRAY)
        {
            fprintf(stderr, "type error: cannot print \n");
            expr_print(s->expr);
            typerr++;
        }
        type_delete(t);
        if (s->expr->next)
        {
            t = expr_typecheck(s->expr->next);
        }
        break;
    case STMT_RETURN:
        fflush(stdout);
        if (s->expr->kind != TYPE_VOID)
        {
            t = expr_typecheck(s->expr); // typecheck the expression itself, typechecking return statements wrt function happens in function declarations
            type_delete(t);
        }
        break;
    case STMT_FOR:
        t1 = expr_typecheck(s->init_expr);
        if (s->expr)
        {
            t2 = expr_typecheck(s->expr);
            if (t2->kind != TYPE_BOOLEAN)
            {
                fprintf(stderr, "type error: second expression in for loop has to be boolean\n");
                typerr++;
            }
        }
        t3 = expr_typecheck(s->next_expr);
        stmt_typecheck(s->body);
        if (t1)
            type_delete(t1);
        //if (t2) type_delete(t2);
        if (t3)
            type_delete(t3);
        break;
    case STMT_IF_ELSE:
        t = expr_typecheck(s->expr);
        if (t->kind != TYPE_BOOLEAN)
        {
            fprintf(stderr, "type error: if statement condition has to be boolean\n");
            typerr++;
        }
        type_delete(t);
        stmt_typecheck(s->body);
        stmt_typecheck(s->else_body);

        break;
    }
    stmt_typecheck(s->next);
}