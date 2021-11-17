%{
#include <stdio.h>
#define YYDEBUG 1
#include "csua.h"    

int yyerror(char const *str);
int yylex();

%}
%union{
    int iv;
    double dv;
    char *name;
    Expression* expression;
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
%token TRUE_T
%token FALSE_T
%token BOOLEAN_T
%token INT_T
%token DOUBLE_T
%token STRING_T

%type <expression> expression assignment_expression logical_or_expression
                 logical_and_expression equality_expression relational_expression
                 additive_expression multiplicative_expression unary_expression
                 postfix_expression primary_expression
                 

%%
translation_unit
	: statement_list
	;

statement_list
        : statement
        | statement_list statement
        ;

statement
	: expression SEMICOLON 
        {
    CS_Compiler* compiler = cs_get_current_compiler();
    compiler->expr_list = cs_chain_expression_list(compiler->expr_list, $1);
        }
	;

expression
	: assignment_expression 
         { 
             Expression* expr = $1;
             printf("type = %d\n", expr->kind);
             $$ = $1;
         }
	;


assignment_expression
        : logical_or_expression
        | postfix_expression assignment_operator assignment_expression {printf("assign\n");}
        ;
assignment_operator
        : ASSIGN_T        { printf("=\n"); }
        | ADD_ASSIGN_T    { printf("+=\n"); }
        | SUB_ASSIGN_T    { printf("-=\n"); }
        | MUL_ASSIGN_T    { printf("*=\n"); }
        | DIV_ASSIGN_T    { printf("/=\n"); }
        | MOD_ASSIGN_T    { printf("%% =\n"); }
        ;

logical_or_expression
        : logical_and_expression
        | logical_or_expression LOGICAL_OR logical_and_expression { printf("LOR\n"); }
        ;
logical_and_expression
        : equality_expression
        | logical_and_expression LOGICAL_AND equality_expression  { printf("LAND\n"); }
        ;

equality_expression
        : relational_expression
        | equality_expression EQ relational_expression { printf("EQ\n"); }
        | equality_expression NE relational_expression { printf("NE\n"); }
        ;

relational_expression
        : additive_expression
        | relational_expression GT additive_expression { printf("gt\n"); }
        | relational_expression GE additive_expression { printf("ge\n"); }
        | relational_expression LT additive_expression { printf("lt\n"); }
        | relational_expression LE additive_expression { printf("ge\n"); }
        ;

additive_expression
        : multiplicative_expression
        | additive_expression ADD multiplicative_expression  { printf("add\n"); $$ = cs_create_binary_expression(ADD_EXPRESSION, $1, $3); }
        | additive_expression SUB multiplicative_expression  { printf("sub\n"); }
        ;

multiplicative_expression
        : unary_expression
        | multiplicative_expression MUL unary_expression { printf("mul\n"); }
        | multiplicative_expression DIV unary_expression { printf("div\n"); }
        | multiplicative_expression MOD unary_expression { printf("mod\n"); }
        ;

unary_expression
        : postfix_expression   
        | SUB unary_expression          { printf("unary sub\n"); }
        | EXCLAMATION unary_expression  { printf("unary exclamation\n"); }
        ;

postfix_expression
        : primary_expression
        | postfix_expression LP RP     { printf("function call\n"); }
        | postfix_expression INCREMENT { printf("increment\n"); }
        | postfix_expression DECREMENT { printf("decrement\n"); }
        ;

primary_expression
	: LP expression RP { printf("LP expr RP\n"); }
	| IDENTIFIER  { printf("IDENTIFIER\n"); }
	| INT_LITERAL { printf("INT_LITERAL\n");}
	| DOUBLE_LITERAL { printf("DOUBLE_LITERAL\n");}
	| TRUE_T      { printf("TRUE_T\n"); $$ = cs_create_boolean_expression(CS_TRUE);}
	| FALSE_T     { printf("FALSE_T\n"); $$ = cs_create_boolean_expression(CS_FALSE);}
	;
%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}
