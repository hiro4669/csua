#include <stdio.h>
#include <string.h>
#include "csua.h"
#include "../memory/MEM.h"

static MEM_Storage storage = NULL;

static void init_storage() {    
    if (storage == NULL) { 
#ifdef STORAGE                
        storage = MEM_open_storage(0);
        printf("init_storage\n");
#else
        storage = cs_get_current_compiler()->storage;
        
#endif
    }
}

static void* cs_malloc(size_t size) {
    init_storage();
    return MEM_storage_malloc(storage, size);
}

static Expression* cs_create_expression(ExpressionKind ekind) {
    Expression* expr = (Expression*)cs_malloc(sizeof(Expression));    
    expr->kind = ekind;
    return expr;
}


Expression* cs_create_int_expression(int v) {
    Expression* expr = cs_create_expression(INT_EXPRESSION);
    expr->u.int_value = v;
    return expr;
}

void delete_storage() {
#ifdef STORAGE            
    if (storage != NULL) {
        MEM_dispose(storage);
    }
#endif            
}

ExpressionList* cs_chain_expression_list(ExpressionList* list, Expression* expr) {
    ExpressionList* p = list;
    ExpressionList* nlist= (ExpressionList*)MEM_storage_malloc(storage, sizeof(ExpressionList));    
    nlist->next = NULL;
    nlist->expression = expr;    
    if (p != NULL) {
        while (p->next) p = p->next;
        p->next = nlist;
        return list;
    } 
    return nlist;
    
}

Expression* cs_create_double_expression(double v) {
    Expression* expr = cs_create_expression(DOUBLE_EXPRESSION);
    expr->u.double_value = v;
    return expr;
}

Expression* cs_create_boolean_expression(CS_Boolean v) {
    Expression* expr = cs_create_expression(BOOLEAN_EXPRESSION);
    expr->u.boolean_value = v;
    return expr;
}

Expression* cs_create_identifier_expression(char* identifier) {
    Expression* expr = cs_create_expression(IDENTIFIER_EXPRESSION);
    expr->u.identifier.name = identifier;
    return expr;
}

Expression* cs_create_inc_dec_expression(Expression* id_expr, ExpressionKind inc_dec) {
    Expression* expr = cs_create_expression(inc_dec);
    expr->u.inc_dec = id_expr;
    return expr;
}

// args is argument not yet
Expression* cs_create_function_call_expression(Expression* function, void* args) {
    Expression* expr = cs_create_expression(FUNCTION_CALL_EXPRESSION);
    expr->u.function_call_expression.function = function;
    return expr;
}

Expression* cs_create_minus_expression(Expression* operand) {
    Expression* expr = cs_create_expression(MINUS_EXPRESSION);
    expr->u.minus_expression = operand;
    return expr;
}

Expression* cs_create_logical_not_expression(Expression* operand) {
    Expression* expr = cs_create_expression(LOGICAL_NOT_EXPRESSION);
    expr->u.logical_not_expression = operand;
    return expr;
}

Expression* cs_create_binary_expression(ExpressionKind kind, Expression* left, Expression* right) {
    Expression* expr = cs_create_expression(kind);
    expr->u.binary_expression.left = left;
    expr->u.binary_expression.right = right;
    return expr;   
}

Expression* cs_create_assignment_expression(Expression *left, AssignmentOperator aope, Expression* operand) {
    Expression* expr = cs_create_expression(ASSIGN_EXPRESSION);
    expr->u.assignment_expression.aope = aope;
    expr->u.assignment_expression.left = left;
    expr->u.assignment_expression.right = operand;
    return expr;            
}
        
        
char* cs_create_identifier(const char* str) {
    char* new_char;
    new_char = (char*)cs_malloc(strlen(str) + 1);
    strcpy(new_char, str);
    return new_char;
}


/* For Statement */
static Statement* cs_create_statement(StatementType type) {
    //Expression* expr = (Expression*)cs_malloc(sizeof(Expression));   
    Statement* stmt = (Statement*)cs_malloc(sizeof(Statement));
    stmt->type = type;
    return stmt;    
}

Statement* cs_create_expression_statement(Expression* expr) {
    Statement* stmt = cs_create_statement(EXPRESSION_STATEMENT);
    stmt->u.expression_s = expr;
    return stmt;
}


TypeSpecifier* cs_create_type_specifier(CS_BasicType type) {
    TypeSpecifier* ts = (TypeSpecifier*)cs_malloc(sizeof(TypeSpecifier));
    ts->basic_type = type;

    return ts;
}

static Declaration* cs_create_declaration(CS_BasicType type, char* name, Expression* initializer) {
    Declaration* decl = (Declaration*)cs_malloc(sizeof(Declaration));
    decl->type = cs_create_type_specifier(type);
    decl->name = name;
    decl->initializer = initializer;
    decl->index = -1;
    decl->is_local = CS_TRUE; // initialize
    return decl;        
}

Statement* cs_create_declaration_statement(CS_BasicType type, char* name, Expression* initializer) {
    Statement* stmt = cs_create_statement(DECLARATION_STATEMENT);
    stmt->u.declaration_s = cs_create_declaration(type, name, initializer);   
    return stmt;    
}

Statement* cs_create_while_statement(Expression *cond, Block *block) {
    Statement* stmt = cs_create_statement(WHILE_STATEMENT);
    stmt->u.while_s.condition = cond;
    stmt->u.while_s.block = block;

    block->type = WHILE_STATEMENT_BLOCK;
    block->parent.statement.statement = stmt;

    return stmt;

}


StatementList* cs_create_statement_list(Statement* stmt) {
    StatementList* stmt_list = (StatementList*)cs_malloc(sizeof(StatementList));
    stmt_list->stmt = stmt;
    stmt_list->next = NULL;
    return stmt_list;
}

StatementList* cs_chain_statement_list(StatementList* stmt_list, Statement* stmt) {
    StatementList* p = NULL;
    StatementList* nstmt_list = cs_create_statement_list(stmt);
    if (stmt_list == NULL) {
//        fprintf(stderr, "stmt_list is null\n");
        return nstmt_list;
    }   
//    fprintf(stderr, "stmt_list is NOT null\n");
    for (p = stmt_list; p->next; p = p->next);
    p->next = nstmt_list;
    
    return stmt_list;
}

ParameterList* cs_create_parameter(CS_BasicType type, char *identifier) {
    ParameterList *p;
    p = cs_malloc(sizeof(ParameterList));
    p->name = identifier;
    p->type = cs_create_type_specifier(type);
    p->line_number = 0; // ToDo
    p->next = NULL;
    return p;
}

ParameterList* cs_chain_parameter(ParameterList *list, CS_BasicType type, char *identifier) {
    ParameterList *p = NULL;

    for (p = list; p->next; p = p->next);
    p->next = cs_create_parameter(type, identifier);
    return list;
}

Block* cs_open_block() {
    Block *new_block;    
    CS_Compiler *compiler = cs_get_current_compiler();
    new_block = cs_malloc(sizeof(Block));
    new_block->type = UNDEFINED_BLOCK;    
    new_block->outer_block = compiler->current_block;
    new_block->declaration_list = NULL;
    new_block->statement_list = NULL;
    compiler->current_block = new_block;

    return new_block;
}

Block* cs_close_block(Block *block, StatementList *statement_list) {
    CS_Compiler *compiler = cs_get_current_compiler();
    block->statement_list = statement_list;
    compiler->current_block = block->outer_block;
    return block;
}

static FunctionDefinition* create_function_definition(CS_BasicType type, char *name, 
    ParameterList *parameter_list, Block *block) {

    FunctionDefinition *fd;
    CS_Compiler *compiler;

    compiler = cs_get_current_compiler();

    fd = cs_malloc(sizeof(FunctionDefinition));
    fd->type = cs_create_type_specifier(type);
    fd->name = name;
    fd->parameter = parameter_list;
    fd->block = block;
    fd->index = compiler->function_count;
    compiler->function_count++;
    fd->local_variable = NULL;
    fd->local_variable_count = 0;
    fd->next = NULL;

    return fd;
}

void cs_function_define(CS_BasicType type, char *name, ParameterList *parameter_list, Block *block) {
    FunctionDefinition *fd;
    FunctionDefinition *pos;
    CS_Compiler *compiler;

    cs_search_function(name);

    // find from compiler
    if (cs_search_function(name) || cs_search_declaration(name, NULL)) {
        fprintf(stderr, "%s exist as function or definition of root\n", name);
        return;
    }

    fd = create_function_definition(type, name, parameter_list, block);

    

    if (block) {
        block->type = FUNCTION_BLOCK;
        block->parent.function.function = fd;
    }    

    compiler = cs_get_current_compiler();
    if (compiler->function_list) {        
        for (pos = compiler->function_list; pos->next; pos = pos->next);        
        pos->next = fd;
        
    } else {
        compiler->function_list = fd;
    }

}

DeclarationList* cs_create_declaration_list(Declaration* decl) {
    DeclarationList* list = (DeclarationList*)cs_malloc(sizeof(DeclarationList));
    if (list) {
        list->decl = decl;
        list->next = NULL;
        return list;
    } 
    return NULL;
}

