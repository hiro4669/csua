/* 
 * File:   csua.h
 * Author: hiroaki
 *
 * Created on October 1, 2018, 1:06 PM
 */

#ifndef _CSUA_H_
#define _CSUA_H_
#include <stdio.h>
#include "../memory/MEM.h"

typedef struct Expression_tag Expression;
typedef struct Visitor_tag Visitor;
typedef struct MeanVisitor_tag MeanVisitor;
typedef struct CS_Compiler_tag CS_Compiler;
typedef struct FunctionDefinition_tag FunctionDefinition;

typedef struct TypeSpecifier_tag TypeSpecifier;
typedef struct Statement_tag Statement;

typedef enum {
    CS_FALSE = 0,
    CS_TRUE = 1    
} CS_Boolean;

typedef enum {
    CS_INT_TO_DOUBLE = 1,
    CS_DOUBLE_TO_INT,
} CS_CastType;

typedef enum {
    CS_BOOLEAN_TYPE,
    CS_INT_TYPE,
    CS_DOUBLE_TYPE,    
} CS_BasicType;

struct TypeSpecifier_tag {
    CS_BasicType basic_type;
};

typedef struct {
    char          *name;
    TypeSpecifier *type;
    Expression    *initializer;
    int           index;
    CS_Boolean    is_local;
    
} Declaration;

typedef enum {
    BOOLEAN_EXPRESSION = 1,
    DOUBLE_EXPRESSION,
    INT_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    INCREMENT_EXPRESSION,
    DECREMENT_EXPRESSION,
    FUNCTION_CALL_EXPRESSION,
    MINUS_EXPRESSION,
    LOGICAL_NOT_EXPRESSION,
    MUL_EXPRESSION,
    DIV_EXPRESSION,
    MOD_EXPRESSION,
    ADD_EXPRESSION,
    SUB_EXPRESSION,
    GT_EXPRESSION,
    GE_EXPRESSION,
    LT_EXPRESSION,
    LE_EXPRESSION,
    EQ_EXPRESSION,
    NE_EXPRESSION,
    LOGICAL_AND_EXPRESSION,
    LOGICAL_OR_EXPRESSION,
    ASSIGN_EXPRESSION,
    CAST_EXPRESSION,
    EXPRESSION_KIND_PLUS_ONE
} ExpressionKind;

typedef struct ParameterList_tag {
    char                     *name;
    TypeSpecifier            *type;
    int                      line_number;
    struct ParameterList_tag *next;
} ParameterList;

typedef struct ArgumentList_tag {
    Expression *expression;
    struct ArgumentList_tag *next;
} ArgumentList;


typedef struct {
    Expression  *function;    
} FunctionCallExpression;

typedef struct {
    char *name;    
} IdentifierExpression;

typedef struct {
    Expression *left;
    Expression *right;    
} BinaryExpression;

typedef struct {
    CS_CastType ctype;
    Expression *expr;
} CastExpression;

typedef enum {
    ASSIGN = 1,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ASSIGN_PLUS_ONE    
} AssignmentOperator;

typedef struct {
    AssignmentOperator aope;
    Expression         *left;
    Expression         *right;        
} AssignmentExpression;

struct Expression_tag {
    ExpressionKind kind;
    union {
        double                 double_value;
        int                    int_value;
        CS_Boolean             boolean_value;
        IdentifierExpression   identifier;
        Expression             *inc_dec;
        FunctionCallExpression function_call_expression;
        Expression             *minus_expression;
        Expression             *logical_not_expression;        
        BinaryExpression       binary_expression;
        AssignmentExpression   assignment_expression;
        CastExpression         cast_expression;
    } u;
};


/*  Statement */

typedef struct DeclarationList_tag {
    Declaration                *decl;
    struct DeclarationList_tag *next;
} DeclarationList;

typedef struct StatementList_tag {
    Statement *stmt;
    struct StatementList_tag *next;
} StatementList;

typedef enum {
    UNDEFINED_BLOCK = 1,
    FUNCTION_BLOCK,
    WHILE_STATEMENT_BLOCK,
    IF_STATEMENT_BLOCK,
} BlockType;

typedef struct {
    FunctionDefinition *function;
    int                end_label;
} FunctionBlockInfo;

typedef struct {
    Statement* statement;
    int        break_label;    /* not used yet */
    int        continue_label; /* not used yet */
} StatementBlockInfo;


typedef struct Block_tag {
    BlockType         type;
    struct Block_tag *outer_block;
    StatementList    *statement_list;
    DeclarationList  *declaration_list;
    union {
        FunctionBlockInfo function;
        StatementBlockInfo statement;
    } parent;
} Block;

typedef enum {
    EXPRESSION_STATEMENT = 1,
    DECLARATION_STATEMENT,
    WHILE_STATEMENT,
    STATEMENT_TYPE_COUNT_PLUS_ONE
} StatementType;

typedef struct {
    Expression *condition;
    Block      *block;    
} WhileStatement;


struct Statement_tag {
    StatementType type;
    int           line_number;
    union {
        Expression     *expression_s;
        Declaration    *declaration_s;
        WhileStatement while_s; // shold not be pointer
    }u;

};

/* Temporary used */
typedef struct ExpressionList_tag {
    Expression *expression;
    struct ExpressionList_tag *next;
} ExpressionList;

struct FunctionDefinition_tag {
    TypeSpecifier  *type;
    char           *name;
    ParameterList  *parameter;
    Block          *block;
    int            local_variable_count;
    Declaration    **local_variable;
    int            index;
    struct FunctionDefinition_tag *next;
};

struct CS_Compiler_tag {
    MEM_Storage storage;
    ExpressionList  *expr_list; // temporary
    StatementList   *stmt_list;
    DeclarationList *decl_list;
    Block           *current_block;
    int             function_count;
    FunctionDefinition *function_list;
};


/* create.c */
//Expression* cs_create_expression(ExpressionKind ekind);
Expression* cs_create_int_expression(int v);
Expression* cs_create_double_expression(double v);
Expression* cs_create_boolean_expression(CS_Boolean v);
Expression* cs_create_identifier_expression(char* identifier);
Expression* cs_create_inc_dec_expression(Expression* id_expr, ExpressionKind inc_dec);
Expression* cs_create_function_call_expression(Expression* function, void* args);
Expression* cs_create_minus_expression(Expression* operand);
Expression* cs_create_logical_not_expression(Expression* operand);
Expression* cs_create_binary_expression(ExpressionKind kind, Expression* left, Expression* right);
Expression* cs_create_assignment_expression(Expression* left, AssignmentOperator aope, Expression* operand);
void delete_storage();
ExpressionList* cs_chain_expression_list(ExpressionList* list, Expression* expr);
char* cs_create_identifier(const char* str);

Statement* cs_create_expression_statement(Expression* expr);
Statement* cs_create_declaration_statement(CS_BasicType type, char* name, Expression* initializer);
Statement* cs_create_while_statement(Expression *cond, Block *block);
StatementList* cs_create_statement_list(Statement* stmt);
StatementList* cs_chain_statement_list(StatementList* stmt_list, Statement* stmt);
ParameterList* cs_create_parameter(CS_BasicType type, char *identifier);
ParameterList* cs_chain_parameter(ParameterList *list, CS_BasicType type, char *identifier);

DeclarationList* cs_create_declaration_list(Declaration* decl);


Block* cs_open_block();
Block* cs_close_block(Block *block, StatementList *statement_list);
void cs_function_define(CS_BasicType type, char *name, ParameterList *parameter_list, Block *block);

/* interface.c */
CS_Compiler* CS_create_compiler();
void CS_compile(CS_Compiler* compiler, FILE *fin);
void CS_delete_compiler(CS_Compiler* compiler);

/* util.c */
void cs_set_current_compiler(CS_Compiler *compiler);
CS_Compiler* cs_get_current_compiler();
FunctionDefinition* cs_search_function(char *name);
Declaration* cs_search_declaration(char *name, Block *block);
DeclarationList* cs_chain_declaration(DeclarationList* decl_list, Declaration* decl);



#endif /* CSUA_H */

