#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"

CodegenVisitor* create_codegen_visitor(CS_Compiler* compiler, CS_Executable *exec) {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
    
    if (compiler == NULL || exec == NULL) {
        fprintf(stderr, "Compiler or Executable is NULL\n");
        exit(1);
    }
    
    CodegenVisitor* visitor = (CodegenVisitor*)MEM_malloc(sizeof(CodegenVisitor));
    visitor->compiler = compiler;
    visitor->exec = exec;

    enter_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    
    memset(enter_expr_list, 0, sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    memset(leave_expr_list, 0, sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    memset(enter_stmt_list, 0, sizeof(visit_expr) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    memset(leave_stmt_list, 0, sizeof(visit_expr) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    
    
    ((Visitor*)visitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)visitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)visitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)visitor)->leave_stmt_list = leave_stmt_list;
    
    
    
    
    return visitor;
}
