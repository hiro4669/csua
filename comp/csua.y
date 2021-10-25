%{
#include <stdio.h>
#define YYDEBUG 1

int yyerror(char const *str);
int yylex();


%}
%union{
    int iv;
    double dv;
    char *name;
}


%token LP
%token RP
%token LC
%token RC
%token COMMA
%token LOGICAL_AND
%token LOGICAL_OR
%token EQ
%token ASSIGN_T
%token NE
%token GT
%token GE
%token LE
%token LT
%token SEMICOLON
%token COLON
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD
%token ADD_ASSIGN_T
%token SUB_ASSIGN_T
%token MUL_ASSIGN_T
%token DIV_ASSIGN_T
%token MOD_ASSIGN_T
%token INCREMENT
%token DECREMENT
%token EXCLAMATION
%token DOT

%token INT_LITERAL
%token DOUBLE_LITERAL
%token IDENTIFIER


%token IF
%token ELSE
%token ELSIF
%token WHILE
%token FOR
%token RETURN
%token BREAK
%token CONTINUE
%token TRUE
%token FALSE
%token BOOLEAN_T
%token INT_T
%token DOUBLE_T
%token STRING_T


%%
translation_unit
	: statement
	;
statement
	: IDENTIFIER LP RP SEMICOLON
	;
%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}
