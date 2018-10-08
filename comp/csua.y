%{
#include <stdio.h>
#define YYDEBUG 1
#include "csua.h"    
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

%token <iv>   INT_LITERAL
%token <dv>   DOUBLE_LITERAL
%token <name> IDENTIFIER


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
        | logical_or_expression LOGICAL_OR logical_and_expression { $$ = cs_create_binary_expression(LOGICAL_OR_EXPRESSION, $1, $3);  }
        ;
logical_and_expression
        : equality_expression
        | logical_and_expression LOGICAL_AND equality_expression  { $$ = cs_create_binary_expression(LOGICAL_AND_EXPRESSION, $1, $3);  }
        ;

equality_expression
        : relational_expression
        | equality_expression EQ relational_expression { $$ = cs_create_binary_expression(EQ_EXPRESSION, $1, $3);  }
        | equality_expression NE relational_expression { $$ = cs_create_binary_expression(NE_EXPRESSION, $1, $3);  }
        ;

relational_expression
        : additive_expression
        | relational_expression GT additive_expression { $$ = cs_create_binary_expression(GT_EXPRESSION, $1, $3); }
        | relational_expression GE additive_expression { $$ = cs_create_binary_expression(GE_EXPRESSION, $1, $3); }
        | relational_expression LT additive_expression { $$ = cs_create_binary_expression(LT_EXPRESSION, $1, $3); }
        | relational_expression LE additive_expression { $$ = cs_create_binary_expression(LE_EXPRESSION, $1, $3); }
        ;

additive_expression
        : multiplicative_expression
        | additive_expression ADD multiplicative_expression  { $$ = cs_create_binary_expression(ADD_EXPRESSION, $1, $3); }
        | additive_expression SUB multiplicative_expression  { $$ = cs_create_binary_expression(SUB_EXPRESSION, $1, $3); }
        ;

multiplicative_expression
        : unary_expression
        | multiplicative_expression MUL unary_expression { $$ = cs_create_binary_expression(MUL_EXPRESSION, $1, $3); }
        | multiplicative_expression DIV unary_expression { $$ = cs_create_binary_expression(DIV_EXPRESSION, $1, $3); }
        | multiplicative_expression MOD unary_expression { $$ = cs_create_binary_expression(MOD_EXPRESSION, $1, $3); }
        ;

unary_expression
        : postfix_expression   
        | SUB unary_expression          { $$ = cs_create_minus_expression($2); }
        | EXCLAMATION unary_expression  { $$ = cs_create_logical_not_expression($2); }
        ;

postfix_expression
        : primary_expression
        | postfix_expression LP RP     { $$ = cs_create_function_call_expression($1, NULL); }
        | postfix_expression INCREMENT { $$ = cs_create_inc_dec_expression($1, INCREMENT_EXPRESSION);}
        | postfix_expression DECREMENT { $$ = cs_create_inc_dec_expression($1, DECREMENT_EXPRESSION);}
        ;

primary_expression
	: LP expression RP { $$ = $2;}
	| IDENTIFIER       { $$ = cs_create_identifier_expression($1); }
	| INT_LITERAL      { $$ = cs_create_int_expression($1); }
	| DOUBLE_LITERAL   { $$ = cs_create_double_expression($1); }
	| TRUE_T           { $$ = cs_create_boolean_expression(CS_TRUE); }
	| FALSE_T          { $$ = cs_create_boolean_expression(CS_FALSE); }
	;
%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}
