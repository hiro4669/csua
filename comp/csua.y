%{
#include <stdio.h>
#define YYDEBUG 1
#include "csua.h"    
%}
%union{
    int                iv;
    double             dv;
    char               *name;
    ParameterList      *parameter_list;
    ArgumentList       *argument_list;
    Expression         *expression;
    Block              *block;
    Statement          *statement;
    StatementList      *statement_list;
    AssignmentOperator assignment_operator;
    CS_BasicType       type_specifier;    
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
                 
%type <argument_list> argument_list                 
%type <block> block
%type <statement_list> statement_list
%type <parameter_list> parameter_list                 
%type <assignment_operator> assignment_operator
%type <type_specifier> type_specifier
%type <statement> statement declaration_statement while_statement

%%
translation_unit
        : definition_or_statement  { /* printf("definition_or_statement\n"); */}
        | translation_unit definition_or_statement { /*printf("unit definition\n"); */}
	;
definition_or_statement
        : function_definition
        | statement  
        {
           //printf("match statement\n");
           CS_Compiler* compiler = cs_get_current_compiler();
           if (compiler) {
               compiler->stmt_list = cs_chain_statement_list(compiler->stmt_list, $1);
           }
        }
        ;

function_definition
        : type_specifier IDENTIFIER LP RP SEMICOLON 
        { 
                //printf("function_definition1 \n"); 
                CS_Compiler* compiler = cs_get_current_compiler();
                cs_function_define($1, $2, NULL, NULL);
        }
        | type_specifier IDENTIFIER LP parameter_list RP SEMICOLON 
        { 
                //printf("function definision2 \n"); 
                CS_Compiler* compiler = cs_get_current_compiler();
                cs_function_define($1, $2, $4, NULL);
        }
        | type_specifier IDENTIFIER LP RP block 
        { 
                //printf("function definision3\n");
                CS_Compiler* compiler = cs_get_current_compiler();                
                cs_function_define($1, $2, NULL, $5);

        }
        | type_specifier IDENTIFIER LP parameter_list RP block 
        { 
                //printf("function definision4 \n"); 
                CS_Compiler* compiler = cs_get_current_compiler();
                //printf("function count = %d\n", compiler->function_count);
                cs_function_define($1, $2, $4, $6);
                
                //printf("function count = %d\n", compiler->function_count);

        }
        ;

statement
	: expression SEMICOLON 
        {
    /*
           CS_Compiler* compiler = cs_get_current_compiler();
           if (compiler) {
               compiler->expr_list = cs_chain_expression_list(compiler->expr_list, $1);
           }
     */
            $$ = cs_create_expression_statement($1);
        }
        | declaration_statement 
        { 
                /* printf("declaration_statement\n"); */
                $$ = $1;
        }
        | while_statement

	;
        
declaration_statement
        : type_specifier IDENTIFIER SEMICOLON 
        {
            /* printf("definition\n"); */
            $$ = cs_create_declaration_statement($1, $2, NULL); 
        }
        | type_specifier IDENTIFIER ASSIGN_T expression SEMICOLON 
        {
            $$ = cs_create_declaration_statement($1, $2, $4); 
        }
        ;
        
while_statement
        : WHILE LP expression RP block
        {
                /* printf("while statement\n"); */
                $$ = cs_create_while_statement($3, $5);
        }
        ;


        
type_specifier
        : BOOLEAN_T { $$ = CS_BOOLEAN_TYPE; }
        | INT_T     { $$ = CS_INT_TYPE;     }
        | DOUBLE_T  { $$ = CS_DOUBLE_TYPE;  }
        ;

parameter_list
        : type_specifier IDENTIFIER 
        { 
                /* printf("parameter_list1\n"); */
                $$ = cs_create_parameter($1, $2);
        }
        | parameter_list COMMA type_specifier IDENTIFIER 
        { 
                /* printf("parameter_list2\n"); */
                $$ = cs_chain_parameter($1, $3, $4);
        }
        ;

argument_list
        : assignment_expression 
        { 
                /* printf("argument_list1\n"); */
                $$ = cs_create_argument_list($1);
        }
        | argument_list COMMA assignment_expression 
        { 
                /* printf("argument_list2\n"); */
                $$ = cs_chain_argument_list($1, $3);
        }
        ;

statement_list
        : statement                  
        { 
                /* printf("statement_list1\n"); */
                $$ = cs_create_statement_list($1);
        }
        | statement_list statement   
        { 
                /* printf("statement_list2\n"); */
                $$ = cs_chain_statement_list($1, $2);
        }
        ;

expression
	: assignment_expression 
         { 
             Expression* expr = $1;
             /* printf("type1 = %d\n", expr->kind); */
             $$ = $1;
         }
	;


assignment_expression
        : logical_or_expression
        | postfix_expression assignment_operator assignment_expression 
        {
          $$ = cs_create_assignment_expression($1, $2, $3);
        }
        ;
assignment_operator
        : ASSIGN_T        { $$ = ASSIGN;     }
        | ADD_ASSIGN_T    { $$ = ADD_ASSIGN; }
        | SUB_ASSIGN_T    { $$ = SUB_ASSIGN; }
        | MUL_ASSIGN_T    { $$ = MUL_ASSIGN; }
        | DIV_ASSIGN_T    { $$ = DIV_ASSIGN; }
        | MOD_ASSIGN_T    { $$ = MOD_ASSIGN; }
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
        | postfix_expression LP argument_list RP { $$ = cs_create_function_call_expression($1, $3); }
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
block
        : LC 
        {
                $<block>$ = cs_open_block();
        }
          statement_list RC 
        { 
                //printf("block1\n");
                $<block>$ = cs_close_block($<block>2, $3);
                $$ = cs_close_block($<block>2, $3);
        }
        | LC RC                
        { 
                Block *empty_block = cs_open_block();
                $$ = cs_close_block(empty_block, NULL);
                printf("block2\n");
        }
        ;
%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}
