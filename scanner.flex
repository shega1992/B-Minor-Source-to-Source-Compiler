%option yylineno
%{
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#define MAX_LENGTH 255
int escape_sequence(char ch);
extern void yywarning(const char * );
extern void yyerror(const char * );
int min_flag = 0;	
%}
DIGIT [0-9]
LETTER [a-zA-Z]
%%
(\t|\r|\n|\ )+                         /* skip whitespace */
(\/\/.+)|"/*"([^*]|(\*+[^*/]))*\*+\/  /*C or C++ Style*/
array                                 { return TOKEN_ARRAY_KEYWORD; }
boolean                               { return TOKEN_BOOLEAN_KEYWORD; }
char                                  { return TOKEN_CHAR_KEYWORD; }
else                                  { return TOKEN_ELSE_KEYWORD; }
false                                 { return TOKEN_FALSE_KEYWORD; }
for                                   { return TOKEN_FOR_KEYWORD; }
function                              { return TOKEN_FUNCTION_KEYWORD; }
if                                    { return TOKEN_IF_KEYWORD; }
integer                               { return TOKEN_INTEGER_KEYWORD; }
print                                 { return TOKEN_PRINT_KEYWORD; }
return                                { return TOKEN_RETURN_KEYWORD; }
string                                { return TOKEN_STRING_KEYWORD; }
true                                  { return TOKEN_TRUE_KEYWORD; }
void                                  { return TOKEN_VOID_KEYWORD; }
while                                 { return TOKEN_WHILE_KEYWORD; }
("_"|{LETTER})("_"|{LETTER}|{DIGIT})* { if(strlen(yytext) > MAX_LENGTH)
										{
											yywarning("Warning: identifier is longer than 255 characters");
											yytext[MAX_LENGTH] = '\0';							
										}
										return TOKEN_IDENTIFIER; 
									  }
0|[1-9]{DIGIT}*                       { 
										if(min_flag)
										{
											char * buffer = (char*)malloc(strlen(yytext) + 2);
											buffer[0] = '-';
											strcpy(buffer + 1, yytext);

											if (strtol(buffer, NULL, 10) == LONG_MIN && errno == ERANGE)        
											{
												yyerror("Scan error: integer underflow");
												free(buffer);
												errno = min_flag = 0; 
												return TOKEN_ERROR;
									    	}
									    	else
									    	{
									    		free(buffer);
									    		min_flag = 0;
									    		return TOKEN_INTEGER_CONSTANT;
									    	} 
                                        		
										}
										else
										{
											if (strtol(yytext, NULL, 10) == LONG_MAX && errno == ERANGE)   
                                        	{
                                        		yyerror("Scan error: integer overflow");
                                        		errno = 0; 
                                        		return TOKEN_ERROR;
                                        	}
                                        	else
                                        		return TOKEN_INTEGER_CONSTANT;
										}                                                         
                                      } 
'(\\?.?)'							  {
										if (strcmp(yytext, "''") == 0) 
										{
											yyerror("Scan error: empty character constant"); 
											return TOKEN_ERROR;
										}
                                        else if (strcmp(yytext, "'\\'") == 0) 
                                        {
                                        	yyerror("Scan error: invalid character");
                                        	yywarning("Note: maybe you meant some escape-sequence"); 
                                        	return TOKEN_ERROR;	
                                        }
                                        else if(yytext[1] == '\\')
										{
    										if(escape_sequence(yytext[2]))
       											return TOKEN_CHARACTER_CONSTANT;
    										else
    										{
        										yyerror("Scan error: unknown escape sequence");
         										return TOKEN_ERROR;
     										}
										}
                                        else
                                        	return TOKEN_CHARACTER_CONSTANT;
									  }
\"(\\.|[^"\\\n])*\"           	      { 
										/* yytext:
											'"' - position 0
											string - from 1 to MAX_LENGTH(including)
											"'" - position MAX_LENGTH + 1
											'\0' - position MAX_LENGTH + 2
										*/
										if(strlen(yytext) > MAX_LENGTH + 2) 
										{
											yywarning("Warning: string is longer than 255 characters");
											yytext[MAX_LENGTH + 1] = '"';	
											yytext[MAX_LENGTH + 2] = '\0';							
										}  
										return TOKEN_STRING_LITERAL; 
									  }
\+\+                               	  { return TOKEN_INCREMENT_OP; }
--									  { return TOKEN_DECREMENT_OP; }
\+									  { return TOKEN_ADD_OP; }
-									  { min_flag = 1; return TOKEN_SUB_OP; }
\*									  { return TOKEN_MUL_OP; }
\/									  { return TOKEN_DIV_OP; }
\%									  { return TOKEN_MOD_OP; }
\^                   				  { return TOKEN_EXP_OP; }
\<                                    { return TOKEN_LESS_THAN; }
\<=									  { return TOKEN_LESS_THAN_EQUAL; }
>									  { return TOKEN_GREATER_THAN; }
>=									  { return TOKEN_GREATER_THAN_EQUAL; }
==									  { return TOKEN_EQUAL; }
!=                                    { return TOKEN_NOT_EQUAL; }
!									  { return TOKEN_LOG_NOT; }
&&									  { return TOKEN_LOG_AND; }
\|\|                                  { return TOKEN_LOG_OR; }
=                                     { return TOKEN_ASSIGNMENT_OP; }	
:									  { return TOKEN_COLON; }
;									  { return TOKEN_SEMICOLON; }
,									  { return TOKEN_COMMA; }
\{									  { return TOKEN_LBRACE; }
\}									  { return TOKEN_RBRACE; }
\[									  { return TOKEN_LBRACKET; }
\]									  { return TOKEN_RBRACKET; }
\(									  { return TOKEN_LPARENTHESIS; }
\)									  { return TOKEN_RPARENTHESIS; }
.                                     { if(strcmp(yytext, "\"") == 0)
											yyerror("Scan error: missing terminating \" character");
										else if(strcmp(yytext, "'") == 0)
											yyerror("Scan error: missing terminating ' character");
										return TOKEN_ERROR; 
									  }
%%
int escape_sequence(char ch)
{
    switch(ch)
    {
        case 'n':
            yytext[1] = '\n';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        case 'r':
            yytext[1] = '\r';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        case 't':
            yytext[1] = '\t';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        case '0':
            yytext[1] = '\0';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        case '\\':
            yytext[1] = '\\';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        case '\'':
            yytext[1] = '\'';
            yytext[2] = '\'';
            yytext[3] = '\0';
            return 1;
        default:
            return 0;
    }
}
int yywrap() { return 1; }
