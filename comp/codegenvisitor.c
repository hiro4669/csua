#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "csua.h"
#include "visitor.h"
#include "../svm/svm.h"




static void enter_castexpr(Expression* expr, Visitor* visitor) {    
}
static void leave_castexpr(Expression* expr, Visitor* visitor) {    
}

static void enter_boolexpr(Expression* expr, Visitor* visitor) {
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {    
}

static void enter_intexpr(Expression* expr, Visitor* visitor) {    
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {    
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {    
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {        
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {        
}


static void enter_addexpr(Expression* expr, Visitor* visitor) {
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
}

static void enter_subexpr(Expression* expr, Visitor* visitor) {
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
}

static void enter_divexpr(Expression* expr, Visitor* visitor) {
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
}

static void enter_modexpr(Expression* expr, Visitor* visitor) {
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
}

static void enter_gtexpr(Expression* expr, Visitor* visitor) {
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
}

static void enter_incexpr(Expression* expr, Visitor* visitor) {
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
}

static void enter_assignexpr(Expression* expr, Visitor* visitor) {
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
}

static void enter_whilestmt(Statement* stmt, Visitor* visitor) {
}

static void leave_whilestmt(Statement* stmt, Visitor* visitor) {
}


static void enter_func(FunctionDefinition* func, Visitor* visitor) {
}
static void leave_func(FunctionDefinition* func, Visitor* visitor) {
}

static void init_visit_stmt_functions(visit_stmt *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;    
}

static void init_visit_expr_functions(visit_expr *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;
}

CodegenVisitor* create_codegen_visitor(CS_Compiler* compiler, CS_Executable* exec) {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;

    if (!compiler || !exec) {
        fprintf(stderr, "Compiler or Executable is NULL\n");
        exit(1);
    }

    CodegenVisitor* cvisitor = (CodegenVisitor*)MEM_malloc(sizeof(CodegenVisitor));
    cvisitor->compiler = compiler;
    cvisitor->exec = exec;

    enter_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);

    init_visit_expr_functions(enter_expr_list, EXPRESSION_KIND_PLUS_ONE);
    init_visit_expr_functions(leave_expr_list, EXPRESSION_KIND_PLUS_ONE);
    init_visit_stmt_functions(enter_stmt_list, STATEMENT_TYPE_COUNT_PLUS_ONE);
    init_visit_stmt_functions(leave_stmt_list, STATEMENT_TYPE_COUNT_PLUS_ONE);

    enter_expr_list[BOOLEAN_EXPRESSION]       = enter_boolexpr;
    enter_expr_list[INT_EXPRESSION]           = enter_intexpr;
    enter_expr_list[DOUBLE_EXPRESSION]        = enter_doubleexpr;
    enter_expr_list[IDENTIFIER_EXPRESSION]    = enter_identexpr;    
    enter_expr_list[ADD_EXPRESSION]           = enter_addexpr;
    enter_expr_list[SUB_EXPRESSION]           = enter_subexpr;
    enter_expr_list[MUL_EXPRESSION]           = enter_mulexpr;
    enter_expr_list[DIV_EXPRESSION]           = enter_divexpr;
    enter_expr_list[MOD_EXPRESSION]           = enter_modexpr;    
    enter_expr_list[GT_EXPRESSION]            = enter_gtexpr;
    enter_expr_list[GE_EXPRESSION]            = enter_geexpr;
    enter_expr_list[LT_EXPRESSION]            = enter_ltexpr;
    enter_expr_list[LE_EXPRESSION]            = enter_leexpr;
    enter_expr_list[EQ_EXPRESSION]            = enter_eqexpr;
    enter_expr_list[NE_EXPRESSION]            = enter_neexpr;
    enter_expr_list[LOGICAL_AND_EXPRESSION]   = enter_landexpr;
    enter_expr_list[LOGICAL_OR_EXPRESSION]    = enter_lorexpr;
    enter_expr_list[INCREMENT_EXPRESSION]     = enter_incexpr;
    enter_expr_list[DECREMENT_EXPRESSION]     = enter_decexpr;
    enter_expr_list[MINUS_EXPRESSION]         = enter_minusexpr;
    enter_expr_list[LOGICAL_NOT_EXPRESSION]   = enter_lognotexpr;
    enter_expr_list[ASSIGN_EXPRESSION]        = enter_assignexpr;
    enter_expr_list[FUNCTION_CALL_EXPRESSION] = enter_funccallexpr;
    enter_expr_list[CAST_EXPRESSION]          = enter_castexpr;
    
    enter_stmt_list[EXPRESSION_STATEMENT]     = enter_exprstmt;
    enter_stmt_list[DECLARATION_STATEMENT]    = enter_declstmt;
    enter_stmt_list[WHILE_STATEMENT]          = enter_whilestmt;

    leave_expr_list[BOOLEAN_EXPRESSION]       = leave_boolexpr;
    leave_expr_list[INT_EXPRESSION]           = leave_intexpr;
    leave_expr_list[DOUBLE_EXPRESSION]        = leave_doubleexpr;
    leave_expr_list[IDENTIFIER_EXPRESSION]    = leave_identexpr;    
    leave_expr_list[ADD_EXPRESSION]           = leave_addexpr;
    leave_expr_list[SUB_EXPRESSION]           = leave_subexpr;
    leave_expr_list[MUL_EXPRESSION]           = leave_mulexpr;
    leave_expr_list[DIV_EXPRESSION]           = leave_divexpr;
    leave_expr_list[MOD_EXPRESSION]           = leave_modexpr;    
    leave_expr_list[GT_EXPRESSION]            = leave_gtexpr;
    leave_expr_list[GE_EXPRESSION]            = leave_geexpr;
    leave_expr_list[LT_EXPRESSION]            = leave_ltexpr;
    leave_expr_list[LE_EXPRESSION]            = leave_leexpr;
    leave_expr_list[EQ_EXPRESSION]            = leave_eqexpr;
    leave_expr_list[NE_EXPRESSION]            = leave_neexpr;
    leave_expr_list[LOGICAL_AND_EXPRESSION]   = leave_landexpr;
    leave_expr_list[LOGICAL_OR_EXPRESSION]    = leave_lorexpr;
    leave_expr_list[INCREMENT_EXPRESSION]     = leave_incexpr;
    leave_expr_list[DECREMENT_EXPRESSION]     = leave_decexpr;
    leave_expr_list[DECREMENT_EXPRESSION]     = leave_decexpr;
    leave_expr_list[MINUS_EXPRESSION]         = leave_minusexpr;
    leave_expr_list[LOGICAL_NOT_EXPRESSION]   = leave_lognotexpr;
    leave_expr_list[ASSIGN_EXPRESSION]        = leave_assignexpr;
    leave_expr_list[FUNCTION_CALL_EXPRESSION] = leave_funccallexpr;
    leave_expr_list[CAST_EXPRESSION]          = leave_castexpr;
    
    leave_stmt_list[EXPRESSION_STATEMENT]     = leave_exprstmt;
    leave_stmt_list[DECLARATION_STATEMENT]    = leave_declstmt;
    leave_stmt_list[WHILE_STATEMENT]          = leave_whilestmt;

    ((Visitor*)cvisitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)cvisitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)cvisitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)cvisitor)->leave_stmt_list = leave_stmt_list;

    ((Visitor*)cvisitor)->enter_func = enter_func;
    ((Visitor*)cvisitor)->leave_func = leave_func;






    return cvisitor;
}

void delete_codegen_visitor(CodegenVisitor* cvisitor) {
    MEM_free(cvisitor);
}