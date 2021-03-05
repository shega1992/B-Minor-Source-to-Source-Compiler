#include <stdio.h>
#include <stdlib.h>
#include "translate.h"

void translate_into_CPP(struct decl *d)
{
    if((fp = fopen("prog.cpp", "w")) == NULL)
    {
        fprintf(stderr, "Can't create prog.c\n");
        exit(EXIT_FAILURE);
    }
    //fp = stdout;
    fprintf(fp,"#include <iostream>\n");
    fprintf(fp,"#include <string>\n");
    fprintf(fp,"#include <cmath>\n\n");
    fprintf(fp,"using std::cout;\n");
    fprintf(fp,"using std::string;\n\n");
    long indent = 0;

    decl_translate(d, indent);
}

void decl_translate(struct decl *d, long indent)
{
    if(!d) return;
    (d->type->kind == TYPE_FUNCTION && !strcmp(d->name,"main"))? fprintf(fp,"int ") :
                                                           type_translate(d->type);
    fprintf(fp,"%s", d->name);
    if(d->value)
    {
        if(d->type->kind == TYPE_ARRAY)
        {
            struct type * t = d->type;
            while(t->kind == TYPE_ARRAY)
            {
                fprintf(fp,"[");
                if (t->size) fprintf(fp,"%d", t->size);
                fprintf(fp,"]");
                t = t->subtype;
            }
            struct expr * e = d->value;
            fprintf(fp," = ");
            fprintf(fp,"{");
            while(e)
            {
                expr_translate(e);
                if(e->right)
                    fprintf(fp,", ");
                e = e->right;
            }
            fprintf(fp,"}");
        }
        else
        {
            fprintf(fp, " = ");
            expr_translate(d->value);
        }
        fprintf(fp,";\n");
    }
    else if(d->code)
    {
        fprintf(fp,"(");
        param_list_translate(d->type->params);
        fprintf(fp,")\n");
        fprintf(fp,"{\n");
        stmt_translate(d->code, indent + 4);
        fprintf(fp,"}\n\n");
    }
    else
    {
        if(d->type->kind == TYPE_FUNCTION)
        {
            fprintf(fp,"(");
            param_list_translate(d->type->params);
            fprintf(fp,")");
            (!strcmp(d->name, "main"))? fprintf(fp,"\n{\n}\n\n") : fprintf(fp,";\n");

        }
        else if(d->type->kind == TYPE_ARRAY)
        {
            struct type * t = d->type;
            while(t->kind == TYPE_ARRAY)
            {
                fprintf(fp,"[");
                if (t->size) fprintf(fp,"%d", t->size);
                fprintf(fp,"]");
                t = t->subtype;
            }
            fprintf(fp,";\n");
        }
        else
           fprintf(fp,";\n");
    }
    decl_translate(d->next, indent);
}

void stmt_translate(struct stmt *s, long indent)
{
    if(!s) return;
    switch(s->kind)
    {
        case STMT_DECL:
            print_tabs(indent);
            decl_translate(s->decl, indent);
            break;
        case STMT_EXPR:
            print_tabs(indent);
            expr_translate(s->expr);
            fprintf(fp,";\n");
            break;
        case STMT_IF_ELSE:
            print_tabs(indent);
            fprintf(fp,"if(");
            expr_translate(s->expr);
            fprintf(fp,")\n");
            (s->body->kind != STMT_BLOCK)? stmt_translate(s->body, indent + 4) : stmt_translate(s->body, indent);
            if(s->else_body)
            {
                print_tabs(indent);
                fprintf(fp,"else\n");
                (s->else_body->kind != STMT_BLOCK)? stmt_translate(s->else_body, indent + 4) : stmt_translate(s->else_body, indent);
            }
            break;
        case STMT_FOR:
            print_tabs(indent);
            fprintf(fp,"for(");
            expr_translate(s->init_expr);
            fprintf(fp,"; ");
            expr_translate(s->expr);
            fprintf(fp,"; ");
            expr_translate(s->next_expr);
            fprintf(fp, ")\n");
            (s->body->kind != STMT_BLOCK)? stmt_translate(s->body, indent + 4) : stmt_translate(s->body, indent);
            break;
        case STMT_PRINT:
            print_tabs(indent);
            struct expr *ss = s->expr;
            fprintf(fp,"cout << ");
            while(ss)
            {
                expr_translate(ss);
                if(ss->next) fprintf(fp," << ");
                ss = ss->next;
            }
            fprintf(fp,";\n");
            break;
        case STMT_RETURN:
            print_tabs(indent);
            fprintf(fp,"return ");
            expr_translate(s->expr);
            fprintf(fp,";\n");
            break;
        case STMT_BLOCK:
            print_tabs(indent);
            fprintf(fp,"{\n");
            stmt_translate(s->body, indent + 4);
            print_tabs(indent);
            fprintf(fp,"}\n");
            break;
    }
    stmt_translate(s->next, indent);
}

void expr_translate(struct expr *e)
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
        expr_translate(e->left);
        fprintf(fp," = ");
        expr_translate(e->right);
        break;
    case EXPR_OR:
        expr_translate(e->left);
        fprintf(fp," || ");
        expr_translate(e->right);
        break;
    case EXPR_AND:
        expr_translate(e->left);
        fprintf(fp," && ");
        expr_translate(e->right);
        break;
    case EXPR_GT:
        expr_translate(e->left);
        fprintf(fp," > ");
        expr_translate(e->right);
        break;
    case EXPR_GTE:
        expr_translate(e->left);
        fprintf(fp," >= ");
        expr_translate(e->right);
        break;
    case EXPR_LT:
        expr_translate(e->left);
        fprintf(fp," < ");
        expr_translate(e->right);
        break;
    case EXPR_LTE:
        expr_translate(e->left);
        fprintf(fp," <= ");
        expr_translate(e->right);
        break;
    case EXPR_EQ:
        expr_translate(e->left);
        fprintf(fp," == ");
        expr_translate(e->right);
        break;
    case EXPR_NEQ:
        expr_translate(e->left);
        fprintf(fp," != ");
        expr_translate(e->right);
        break;
    case EXPR_ADD:
        expr_translate(e->left);
        fprintf(fp," + ");
        expr_translate(e->right);
        break;
    case EXPR_SUB:
        expr_translate(e->left);
        fprintf(fp," - ");
        expr_translate(e->right);
        break;
    case EXPR_MUL:
        expr_translate(e->left);
        fprintf(fp," * ");
        expr_translate(e->right);
        break;
    case EXPR_DIV:
        expr_translate(e->left);
        fprintf(fp," / ");
        expr_translate(e->right);
        break;
    case EXPR_MOD:
        expr_translate(e->left);
        fprintf(fp," %% ");
        expr_translate(e->right);
        break;
    case EXPR_EXP:
        fprintf(fp,"pow(");
        expr_translate(e->left);
        fprintf(fp,", ");
        expr_translate(e->right);
        fprintf(fp,")");
        break;
    case EXPR_NOT:
        fprintf(fp," !");
        expr_translate(e->right);
        break;
    case EXPR_NEG:
        fprintf(fp," -");
        expr_translate(e->right);
        break;
    case EXPR_INC:
        expr_translate(e->left);
        fprintf(fp,"++ ");
        break;
    case EXPR_DEC:
        expr_translate(e->left);
        fprintf(fp,"-- ");
        break;
    case EXPR_INTEGER_LITERAL:
        fprintf(fp,"%d", e->integer_value);
        break;
    case EXPR_BOOLEAN_LITERAL:
        switch (e->boolean_value)
        {
        case 0:
            fprintf(fp,"false");
            break;
        case 1:
            fprintf(fp,"true");
            break;
        }
        break;
    case EXPR_STRING_LITERAL:
        fprintf(fp,"%s", e->string_literal);
        break;
    case EXPR_CHAR_LITERAL:
        fprintf(fp,"'");
        switch(e->character_literal)
        {
            case 0: fprintf(fp,"\\0"); break;
            case 9: fprintf(fp,"\\t"); break;
            case 10: fprintf(fp,"\\n"); break;
            case 13: fprintf(fp,"\\r"); break;
            case 39: fprintf(fp,"\\'"); break;
            case 92: fprintf(fp,"\\\\"); break;
            default: fprintf(fp,"%c", e->character_literal); break;
        }
        fprintf(fp,"'");
        break;
    case EXPR_NAME:
        fprintf(fp,"%s", e->name);
        break;
    case EXPR_CALL:
        expr_translate(e->left);
        fprintf(fp,"(");
        exprs_translate(e->right, 0);
        fprintf(fp,")");
        break;
    case EXPR_GROUP:
        fprintf(fp,"(");
        expr_translate(e->right);
        fprintf(fp,")");
        break;
    case EXPR_SUBSCRIPT:
        expr_translate(e->left);
        fprintf(fp,"[");
        expr_translate(e->right);
        fprintf(fp,"]");

        // More array accesses for multiple dimensions
        /*struct expr *ee = e->right;
        while (ee->right)
        {
            fprintf(fp,"[");
            expr_translate(ee);
            fprintf(fp,"]");
            ee = ee->right;
        }
        break;*/
    }
}

void exprs_translate(struct expr *e, long indent)
{
    if (!e)
        return;
    expr_translate(e);
    if (e->next)
    {
        fprintf(fp,", ");
    }
    exprs_translate(e->next, indent);
}

void type_translate(struct type *t)
{
    if (!t) return;
    switch(t->kind) {
        case TYPE_VOID:
            fprintf(fp,"void ");
            break;
        case TYPE_BOOLEAN:
            fprintf(fp,"bool ");
            break;
        case TYPE_CHARACTER:
            fprintf(fp,"char ");
            break;
        case TYPE_INTEGER:
            fprintf(fp,"int ");
            break;
        case TYPE_STRING:
            fprintf(fp,"string ");
            break;
        case TYPE_FUNCTION:
            type_translate(t->subtype);
            break;
        case TYPE_ARRAY:
            type_translate(t->subtype);
            break;
    }
}

void param_list_translate(struct param_list *p)
{
    if (!p) return;
    type_translate(p->type);
    fprintf(fp,"%s", p->name);
    if(p->type->kind == TYPE_ARRAY)
        fprintf(fp,"[]");
    if (p->next) fprintf(fp,", ");
    param_list_translate(p->next);
}

void print_tabs(long indent)
{
    long i = 0;
    while(i < indent)
    {
        fprintf(fp," ");
        i++;
    }
}


