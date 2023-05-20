/* 
 * File:   csua.h
 * Author: hiroaki
 *
 * Created on October 1, 2018, 1:06 PM
 */

#ifndef _CSUA_H_
#define _CSUA_H_
#include <stdio.h>
#include <stdint.h>
#include "../memory/MEM.h"

typedef struct Expression_tag Expression;
typedef struct Visitor_tag Visitor;
typedef struct MeanVisitor_tag MeanVisitor;
typedef struct CodegenVisitor_tag CodegenVisitor;
typedef struct CS_Compiler_tag CS_Compiler;

typedef struct TypeSpecifier_tag TypeSpecifier;
typedef struct Statement_tag Statement;

typedef enum {
    CS_FALSE = 0,
    CS_TRUE = 1    
} CS_Boolean;


typedef enum {
    CS_BOOLEAN_TYPE,
    CS_INT_TYPE,
    CS_DOUBLE_TYPE,
    CS_STRING_TYPE,
    CS_BASIC_TYPE_PLUS_ONE,
} CS_BasicType;

typedef enum {
    CS_INT_TO_DOUBLE = 1,
    CS_DOUBLE_TO_INT,               
} CS_CastType;

struct TypeSpecifier_tag {
    CS_BasicType basic_type;
};

typedef struct {
    char          *name;
    TypeSpecifier *type;
    Expression    *initializer;
    int           index;    
} Declaration;

typedef struct {
    char          *name;
    TypeSpecifier *type;
    int            index;   
} FunctionDeclaration;

typedef enum {
    BOOLEAN_EXPRESSION = 1,
    DOUBLE_EXPRESSION,
    INT_EXPRESSION,
    STRING_EXPRESSION,
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



typedef struct {
    Expression  *function;    
} FunctionCallExpression;

typedef struct {
    char *name;    
    CS_Boolean is_function;
    union {
        Declaration         *declaration;
        FunctionDeclaration *function;
    }u;
} IdentifierExpression;

typedef struct {
    Expression *left;
    Expression *right;    
} BinaryExpression;

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
    CS_CastType ctype;
    Expression* expr;    
} CastExpression;

typedef struct {
    AssignmentOperator aope;
    Expression         *left;
    Expression         *right;        
} AssignmentExpression;

struct Expression_tag {
    ExpressionKind kind;
    TypeSpecifier* type; 
    int line_number;
    union {
        double                 double_value;
        int                    int_value;
        char                   *string_value;
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

typedef enum {
    EXPRESSION_STATEMENT = 1,
    DECLARATION_STATEMENT,
    STATEMENT_TYPE_COUNT_PLUS_ONE
} StatementType;


struct Statement_tag {
    StatementType type;
    int           line_number;
    union {
        Expression   *expression_s;
        Declaration  *declaration_s;
    }u;

};

/* Temporary used */
typedef struct ExpressionList_tag {
    Expression *expression;
    struct ExpressionList_tag *next;
} ExpressionList;

typedef struct StatementList_tag {
    Statement *stmt;
    struct StatementList_tag *next;
} StatementList;

typedef struct DeclarationList_tag {
    Declaration* decl;
    struct DeclarationList_tag *next;
} DeclarationList;

typedef struct FunctionDeclarationList_tag {
    FunctionDeclaration                *func;
    struct FunctionDeclarationList_tag *next;
} FunctionDeclarationList;

struct CS_Compiler_tag {
    MEM_Storage      storage;
    ExpressionList          *expr_list; // temporary
    StatementList           *stmt_list;
    DeclarationList         *decl_list;
    FunctionDeclarationList *func_list;
    int current_line;
};

/* For Code Generation */

typedef struct {
    char          *name;
    TypeSpecifier *type;
} CS_Variable;

typedef enum {
    CS_CONSTANT_INT,
    CS_CONSTANT_DOUBLE
} CS_ConstantType;

typedef struct {
    CS_ConstantType type;
    union {
      int    c_int;
      double c_double;
    }u;    
} CS_ConstantPool;


typedef struct {
    uint32_t         constant_pool_count;
    CS_ConstantPool *constant_pool;
    uint32_t         global_variable_count;
    CS_Variable     *global_variable;
    uint32_t        code_size;
    uint8_t         *code;
} CS_Executable;





/* create.c */
//Expression* cs_create_expression(ExpressionKind ekind);
void* cs_malloc(size_t size);
Expression* cs_create_int_expression(int v);
Expression* cs_create_double_expression(double v);
Expression* cs_create_boolean_expression(CS_Boolean v);
Expression* cs_create_identifier_expression(char* identifier);
Expression* cs_create_string_expression(char* str);
Expression* cs_create_inc_dec_expression(Expression* id_expr, ExpressionKind inc_dec);
Expression* cs_create_function_call_expression(Expression* function, void* args);
Expression* cs_create_minus_expression(Expression* operand);
Expression* cs_create_logical_not_expression(Expression* operand);
Expression* cs_create_binary_expression(ExpressionKind kind, Expression* left, Expression* right);
Expression* cs_create_assignment_expression(Expression* left, AssignmentOperator aope, Expression* operand);
Expression* cs_create_cast_expression(CS_CastType ctype, Expression* operand);
void delete_storage();
ExpressionList* cs_chain_expression_list(ExpressionList* list, Expression* expr);
char* cs_create_string(const char* str);
char* cs_create_identifier(const char* str);

Statement* cs_create_expression_statement(Expression* expr);
Statement* cs_create_declaration_statement(CS_BasicType type, char* name, Expression* initializer);
StatementList* cs_create_statement_list(Statement* stmt);


DeclarationList* cs_create_declaration_list(Declaration* decl);
TypeSpecifier* cs_create_type_specifier(CS_BasicType type);

FunctionDeclaration* cs_create_function_declaration(CS_BasicType type, char *name);
FunctionDeclarationList* cs_create_function_declaration_list(FunctionDeclaration* func);


/* interface.c */
CS_Compiler* CS_create_compiler();
CS_Boolean CS_compile(CS_Compiler* compiler, FILE *fin);
void CS_delete_compiler(CS_Compiler* compiler);

/* util.c */
void cs_set_current_compiler(CS_Compiler *compiler);
CS_Compiler* cs_get_current_compiler();
DeclarationList* cs_chain_declaration(DeclarationList* decl_list, Declaration* decl);
StatementList* cs_chain_statement_list(StatementList* stmt_list, Statement* stmt);
FunctionDeclarationList* cs_chain_function_declaration_list(FunctionDeclarationList* func_list, FunctionDeclaration* func);
Declaration* cs_search_decl_in_block();
Declaration* cs_search_decl_global(const char* name);
FunctionDeclaration* cs_search_function(const char* name);


/* scanner.c */
int get_current_line();
#endif /* CSUA_H */

