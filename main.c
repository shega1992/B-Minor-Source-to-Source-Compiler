#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "translate.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno;
extern char *yytext;
extern struct decl* parser_result;

int typerr = 0;
int reserr = 0;

void yyerror(const char * errmsg)
{
	fprintf(stderr, "%s: %s. Line %d\n", errmsg, yytext, yylineno);
}

void yywarning(const char * errmsg)
{
	fprintf(stderr, "%s: %s. Line %d\n", errmsg, yytext, yylineno);
}


int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("\nNot enough arguments. You missed a flag or a file name. \n");
		exit(EXIT_FAILURE);
	}
	if((yyin = fopen(argv[2], "r")) == NULL)
	{
		printf("\nCannot open file %s.\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	yylineno = 1;

	if(!strcmp(argv[1],"-scan"))
    {
        printf("List of tokens in %s:\n", argv[2]);
        enum yytokentype t;
        while((t = yylex()))
        {
            switch(t)
            {
			case TOKEN_ARRAY_KEYWORD: printf("<ARRAY,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_BOOLEAN_KEYWORD: printf("<BOOLEAN,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_CHAR_KEYWORD: printf("<CHAR,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_ELSE_KEYWORD: printf("<ELSE,%d>: %s\n", yylineno, yytext);
				break;
	        case TOKEN_FALSE_KEYWORD: printf("<FALSE,%d>: %s\n", yylineno, yytext);
	        	break;
			case TOKEN_FOR_KEYWORD: printf("<FOR,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_FUNCTION_KEYWORD: printf("<FUNCTION,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_IF_KEYWORD: printf("<IF,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_INTEGER_KEYWORD: printf("<INTEGER,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_PRINT_KEYWORD: printf("<PRINT,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_RETURN_KEYWORD: printf("<RETURN,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_STRING_KEYWORD: printf("<STRING,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_TRUE_KEYWORD: printf("<TRUE,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_VOID_KEYWORD: printf("<VOID,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_WHILE_KEYWORD: printf("<WHILE,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_IDENTIFIER: printf("<IDENTIFIER,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_INTEGER_CONSTANT: printf("<INTEGER CONSTANT,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_CHARACTER_CONSTANT: printf("<CHARACTER CONSTANT,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_STRING_LITERAL: printf("<STRING LITERAL,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_ASSIGNMENT_OP: printf("<ASSIGNMENT OPERATOR,%d>: %s\n", yylineno, yytext);
				break;
            case TOKEN_LOG_OR: printf("<LOGICAL OR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_LOG_AND: printf("<LOGICAL AND,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_EQUAL: printf("<EQUAL,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_NOT_EQUAL: printf("<NOT EQUAL,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_LESS_THAN: printf("<LESS THAN,%d>: %s\n", yylineno, yytext);
                break;
             case TOKEN_LESS_THAN_EQUAL: printf("<LESS THAN EQUAL,%d>: %s\n", yylineno, yytext);
                break;
             case TOKEN_GREATER_THAN: printf("<GREATER THAN,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_GREATER_THAN_EQUAL: printf("<GREATER THAN EQUAL,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_ADD_OP: printf("<ADD OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_SUB_OP: printf("<SUB OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_MUL_OP: printf("<MUL OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_DIV_OP: printf("<DIV OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_MOD_OP: printf("<MOD OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_EXP_OP: printf("<EXP OPERATOR,%d>: %s\n", yylineno, yytext);
                break;
            case TOKEN_LOG_NOT: printf("<LOGICAL NOT,%d>: %s\n", yylineno, yytext);
                break;
			case TOKEN_INCREMENT_OP:  printf("<INCREMENT OPERATOR,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_DECREMENT_OP: printf("<DECREMENT OPERATOR,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_COLON: printf("<COLON,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_SEMICOLON: printf("<SEMICOLON,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_COMMA: printf("<COMMA,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_LBRACE: printf("<LEFT BRACE,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_RBRACE: printf("<RIGHT BRACE,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_LBRACKET: printf("<LEFT BRACKET,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_RBRACKET: printf("<RIGHT BRACKET,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_LPARENTHESIS: printf("<LEFT PARENTHESIS,%d>: %s\n", yylineno, yytext);
				break;
			case TOKEN_RPARENTHESIS: printf("<RIGHT PARENTHESIS,%d>: %s\n", yylineno, yytext);
				break;
            case TOKEN_ERROR: printf("<UNKNOWN TOKEN,%d>: %s\n", yylineno, yytext);
				break;
            }
        }
    }
	else if(!strcmp(argv[1],"-parse"))
        (!yyparse())? puts("Parse success") : puts("Parse failure");
	else if(!strcmp(argv[1],"-resolve"))
    {
        if(!yyparse())
        {
            scope_enter();
            decl_resolve(parser_result, 1);
            if(reserr)
                fprintf(stderr, "resolve error: %i resolve error(s)\n", reserr);
        }
        else
            puts("Parse failure");
    }
	else if(!strcmp(argv[1],"-typecheck"))
    {
        if(!yyparse())
        {
            scope_enter();
            decl_resolve(parser_result, 0);
            if(reserr)
            {
               fprintf(stderr, "resolve error: %i resolve error(s)\n", reserr);
               exit(EXIT_FAILURE);
            }

            decl_typecheck(parser_result);
            if (typerr)
                fprintf(stderr, "type error: %i type error(s)\n", typerr);
            else
                fprintf(stdout, "typecheck successful!\n");
        }
        else
            puts("Parse failure");
    }
	else if(!strcmp(argv[1],"-cpp"))
    {
        if(!yyparse())
        {
            scope_enter();
            decl_resolve(parser_result, 0);
            if(reserr)
            {
                fprintf(stderr, "resolve error: %i resolve error(s)\n", reserr);
                exit(EXIT_FAILURE);
            }

            decl_typecheck(parser_result);
            if (typerr)
                fprintf(stderr, "type error: %i type error(s)\n", typerr);
            else
                translate_into_CPP(parser_result);
        }
        else
            puts("Parse failure");
    }
	else
    {
        fprintf(stderr, "Error. Incorrect flag");
        exit(EXIT_FAILURE);
    }

    return 0;

}
