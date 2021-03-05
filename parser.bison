%{
  #include <string.h>	
  #include "ast.h"

  extern char *yytext; 
  extern int yylex();

  extern void yyerror (const char *);

  struct decl* parser_result;
%}

%token TOKEN_ARRAY_KEYWORD
%token TOKEN_BOOLEAN_KEYWORD
%token TOKEN_CHAR_KEYWORD
%token TOKEN_ELSE_KEYWORD
%token TOKEN_FALSE_KEYWORD
%token TOKEN_FOR_KEYWORD
%token TOKEN_FUNCTION_KEYWORD
%token TOKEN_IF_KEYWORD
%token TOKEN_INTEGER_KEYWORD
%token TOKEN_PRINT_KEYWORD
%token TOKEN_RETURN_KEYWORD
%token TOKEN_STRING_KEYWORD
%token TOKEN_TRUE_KEYWORD
%token TOKEN_VOID_KEYWORD
%token TOKEN_WHILE_KEYWORD
%token TOKEN_IDENTIFIER
%token TOKEN_INTEGER_CONSTANT
%token TOKEN_CHARACTER_CONSTANT
%token TOKEN_STRING_LITERAL
%token TOKEN_ASSIGNMENT_OP
%token TOKEN_LOG_OR
%token TOKEN_LOG_AND
%token TOKEN_EQUAL TOKEN_NOT_EQUAL
%token TOKEN_LESS_THAN TOKEN_LESS_THAN_EQUAL TOKEN_GREATER_THAN TOKEN_GREATER_THAN_EQUAL
%token TOKEN_ADD_OP TOKEN_SUB_OP
%token TOKEN_MUL_OP TOKEN_DIV_OP TOKEN_MOD_OP
%token TOKEN_EXP_OP
%token TOKEN_LOG_NOT
%token TOKEN_INCREMENT_OP
%token TOKEN_DECREMENT_OP
%token TOKEN_COLON
%token TOKEN_SEMICOLON
%token TOKEN_COMMA
%token TOKEN_LBRACE
%token TOKEN_RBRACE
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_LPARENTHESIS
%token TOKEN_RPARENTHESIS
%token TOKEN_ERROR

%union 
{
  struct decl *decl;
	struct stmt *stmt;
	struct expr *expr;
	struct type *type;
	struct param_list *param_list;
	char *name;
  int size;
};

%type <decl> program decl_list decl param
%type <stmt> stmt_list stmt matched unmatched other_stmt
%type <expr> expr expr_list for_expr arr_items literal lor land comp addsub mult expo not postfix grouping brack bracks
%type <type> type array
%type <param_list> param_list 
%type <name> name
%type <size> size

%%

program : decl_list
          { parser_result = $1; }
        ;

decl_list : decl decl_list
            { $$ = $1; $1->next = $2; }
          | %empty { $$ = 0; }
          ;

decl : name TOKEN_COLON type TOKEN_SEMICOLON
       { $$ = decl_create($1,$3,0,0,0); }
     | name TOKEN_COLON type TOKEN_ASSIGNMENT_OP expr TOKEN_SEMICOLON
       { $$ = decl_create($1,$3,$5,0,0); }
     | name TOKEN_COLON type TOKEN_ASSIGNMENT_OP TOKEN_LBRACE arr_items TOKEN_RBRACE TOKEN_SEMICOLON
       { $$ = decl_create($1,$3,$6,0,0); }
     | name TOKEN_COLON TOKEN_FUNCTION_KEYWORD type TOKEN_LPARENTHESIS param_list TOKEN_RPARENTHESIS TOKEN_ASSIGNMENT_OP TOKEN_LBRACE stmt_list TOKEN_RBRACE
       { $$ = decl_create($1, type_create(TYPE_FUNCTION,$4,$6,0), 0, $10, 0 ); }
     | name TOKEN_COLON TOKEN_FUNCTION_KEYWORD type TOKEN_LPARENTHESIS param_list TOKEN_RPARENTHESIS TOKEN_SEMICOLON
       { $$ = decl_create($1, type_create(TYPE_FUNCTION,$4,$6,0), 0, 0, 0 ); }
	   ;

stmt_list : stmt stmt_list
			{ $$ = $1; $1->next = $2; }
		  | %empty { $$ = 0; }
		  ;

stmt: matched {$$ = $1;}
    | unmatched {$$ = $1;}
    ;

matched: TOKEN_IF_KEYWORD TOKEN_LPARENTHESIS expr TOKEN_RPARENTHESIS matched TOKEN_ELSE_KEYWORD matched {$$ =  stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0);}
       | TOKEN_FOR_KEYWORD TOKEN_LPARENTHESIS for_expr TOKEN_SEMICOLON for_expr TOKEN_SEMICOLON for_expr TOKEN_RPARENTHESIS matched {$$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0);}       
       | other_stmt {$$ = $1;}
       ;

unmatched: TOKEN_IF_KEYWORD TOKEN_LPARENTHESIS expr TOKEN_RPARENTHESIS stmt {$$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, 0, 0);}
         | TOKEN_IF_KEYWORD TOKEN_LPARENTHESIS expr TOKEN_RPARENTHESIS matched TOKEN_ELSE_KEYWORD unmatched {$$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0);}
         | TOKEN_FOR_KEYWORD TOKEN_LPARENTHESIS for_expr TOKEN_SEMICOLON for_expr TOKEN_SEMICOLON for_expr TOKEN_RPARENTHESIS unmatched {$$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0);}
         ;

other_stmt: TOKEN_RETURN_KEYWORD expr TOKEN_SEMICOLON {$$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0, 0);}
    | TOKEN_RETURN_KEYWORD TOKEN_SEMICOLON {$$ = stmt_create(STMT_RETURN, 0, 0, expr_create(TYPE_VOID, 0, 0), 0, 0, 0, 0);}
    | TOKEN_PRINT_KEYWORD expr_list TOKEN_SEMICOLON {$$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0, 0);}
    | TOKEN_LBRACE stmt_list TOKEN_RBRACE {$$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0);}
    | decl {$$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0);}
    | expr TOKEN_SEMICOLON {$$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0, 0);}
    ;

for_expr: expr { $$ = $1; }
        | %empty { $$ = 0; }
        ;


expr_list: expr { $$ = $1;}
        |  expr TOKEN_COMMA expr_list { $$ = $1, $1->next = $3; }
        |  %empty { $$ = 0; }
        ; 

expr: expr TOKEN_ASSIGNMENT_OP lor {$$ = expr_create(EXPR_ASSIGN, $1, $3);}
    | lor {$$ = $1;};
    ;

lor: lor TOKEN_LOG_OR land {$$ = expr_create(EXPR_OR, $1, $3);}
   | land {$$ = $1;}
   ;   

land: land TOKEN_LOG_AND comp {$$ = expr_create(EXPR_AND, $1, $3);}
    | comp {$$ = $1;}
    ;

comp: comp TOKEN_GREATER_THAN addsub       
      {$$ = expr_create(EXPR_GT, $1, $3);}
    | comp TOKEN_GREATER_THAN_EQUAL addsub 
      {$$ = expr_create(EXPR_GTE, $1, $3);}
    | comp TOKEN_LESS_THAN addsub 
      {$$ = expr_create(EXPR_LT, $1, $3);}
    | comp TOKEN_LESS_THAN_EQUAL addsub 
      {$$ = expr_create(EXPR_LTE, $1, $3);}
    | comp TOKEN_EQUAL addsub 
      {$$ = expr_create(EXPR_EQ, $1, $3);}
    | comp TOKEN_NOT_EQUAL addsub 
      {$$ = expr_create(EXPR_NEQ, $1, $3);}
    | addsub {$$ = $1;}
    ;

addsub: addsub TOKEN_ADD_OP mult {$$ = expr_create(EXPR_ADD, $1, $3); }
      | addsub TOKEN_SUB_OP mult {$$ = expr_create(EXPR_SUB, $1, $3); }
      | mult {$$ = $1;}
      ;

mult: mult TOKEN_MUL_OP expo {$$ = expr_create(EXPR_MUL, $1, $3);}
    | mult TOKEN_DIV_OP expo {$$ = expr_create(EXPR_DIV, $1, $3);}
    | mult TOKEN_MOD_OP expo {$$ = expr_create(EXPR_MOD, $1, $3);}
    | expo {$$ = $1;}
    ;

expo: expo TOKEN_EXP_OP not {$$ = expr_create(EXPR_EXP, $1, $3);}
    | not {$$ = $1;}
    ;

not: TOKEN_LOG_NOT postfix {$$ = expr_create(EXPR_NOT, 0, $2);}
   | postfix {$$ = $1;}
   | TOKEN_SUB_OP postfix {$$ = expr_create(EXPR_NEG, 0, $2);}
   ;

postfix: postfix TOKEN_INCREMENT_OP {$$ = expr_create(EXPR_INC, $1, 0);}
       | postfix TOKEN_DECREMENT_OP {$$ = expr_create(EXPR_DEC, $1, 0);}
       | grouping {$$ = $1;}
       ;

grouping: TOKEN_LPARENTHESIS expr TOKEN_RPARENTHESIS {$$ = expr_create(EXPR_GROUP, 0, $2);}
        | name bracks {$$ = expr_create(EXPR_SUBSCRIPT, expr_create_name($1), $2);}
        | name TOKEN_LPARENTHESIS expr_list TOKEN_RPARENTHESIS {$$ = expr_create(EXPR_CALL, expr_create_name($1), $3);}
        | literal {$$ = $1;}
      ;

bracks: brack {$$ = $1; }
      | brack bracks {$$ = $1, $1->right = $2;}
      ;

brack: TOKEN_LBRACKET expr TOKEN_RBRACKET {$$ = $2; }
  ;


type: TOKEN_BOOLEAN_KEYWORD { $$ = type_create(TYPE_BOOLEAN,0,0,0); }
	  | TOKEN_CHAR_KEYWORD { $$ = type_create(TYPE_CHARACTER,0,0,0); }
	  | TOKEN_INTEGER_KEYWORD { $$ = type_create(TYPE_INTEGER,0,0,0); }
	  | TOKEN_STRING_KEYWORD { $$ = type_create(TYPE_STRING,0,0,0); }
	  | TOKEN_VOID_KEYWORD { $$ = type_create(TYPE_VOID,0,0,0); }
	  | array type { $$ = $1; $1->subtype = $2; }
	  ;

array: TOKEN_ARRAY_KEYWORD TOKEN_LBRACKET TOKEN_RBRACKET 
       { $$ = type_create(TYPE_ARRAY,0,0,0); }
    |  TOKEN_ARRAY_KEYWORD TOKEN_LBRACKET size TOKEN_RBRACKET 
       { $$ = type_create(TYPE_ARRAY,0,0, $3); }
    ;

arr_items: literal { $$ = $1; }
        |  literal TOKEN_COMMA arr_items { $$ = $1; $1->right = $3; }
        |  %empty {$$ = 0;}
        ;


param_list: param { $$ = param_list_create($1->name,$1->type,0); }
        |   param TOKEN_COMMA param_list { $$ = param_list_create($1->name, $1->type, 0), $$->next = $3;}
        |  %empty {$$ = 0; }
		    ;

param: name TOKEN_COLON type { $$ = decl_create($1,$3,0,0, 0); }
        ;

literal: size { $$ = expr_create_integer_literal($1); }
       | TOKEN_TRUE_KEYWORD { $$ = expr_create_boolean_literal(1); }
       | TOKEN_FALSE_KEYWORD { $$ = expr_create_boolean_literal(0); }
       | TOKEN_CHARACTER_CONSTANT { $$ = expr_create_char_literal(yytext[1]); }
       | TOKEN_STRING_LITERAL { $$ = expr_create_string_literal(strdup(yytext)); }
       | name {$$ = expr_create_name(strdup($1));}
       ;

size: TOKEN_INTEGER_CONSTANT {$$ = atoi(yytext);}
      ;


name: TOKEN_IDENTIFIER { $$ = strdup(yytext); }
      ;
%%
