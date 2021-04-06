#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"


int main(void) {
    
    FILE *fin = fopen("tests/prog1.cs", "r");
    CS_Compiler* compiler = CS_create_compiler();
    CS_compile(compiler, fin);

    printf("temporary end\n");

    
    Visitor* visitor = create_treeview_visitor();
    MeanVisitor* mvisitor = create_mean_visitor();

    printf("--------------\n");
    printf("-- Function Definition --\n");
    FunctionDefinition* function = compiler->function_list;
    while (function) {
        traverse_func(function, (Visitor*)mvisitor);
        function = function->next;
    }
    
    /*
    ExpressionList* expr_list = compiler->expr_list;
    while(expr_list) {
        printf("kind = %d\n", expr_list->expression->kind);
        traverse_expr(expr_list->expression, visitor);
        expr_list = expr_list->next;
    }
    */
    printf("--------------\n");
    printf("-- Statement List --\n");
    StatementList* stmt_list = compiler->stmt_list;
    while(stmt_list) {        
        traverse_stmt(stmt_list->stmt, (Visitor*)mvisitor);
        stmt_list = stmt_list->next;
    }

    printf("************************************\n");

    // for decl test
    DeclarationList* pos;
    for (pos = compiler->decl_list; pos; pos = pos->next) {
        printf("name(%s), type(%d)\n", pos->decl->name, pos->decl->type->basic_type);
    }
    printf("-- Function Definition --\n");
    function = compiler->function_list;
    while (function) {
        traverse_func(function, visitor);
        function = function->next;
    }

    printf("--------------\n");
    printf("-- Statement List --\n");
    stmt_list = compiler->stmt_list;
    while(stmt_list) {        
        traverse_stmt(stmt_list->stmt, visitor);
        stmt_list = stmt_list->next;
    }

    printf("---------------------\n");
    printf("-- Errors --\n");
    show_mean_error(mvisitor);

    DeclarationList* dpos;
    int var_count = 0;
    for (dpos = compiler->decl_list; dpos; dpos = dpos->next) {
        dpos->decl->index = var_count++;
    }
    printf("var count  = %d\n", var_count);
    printf("func count = %d\n", compiler->function_count);
    FunctionDefinition* func;
    for (func = compiler->function_list; func; func = func->next) {
        printf("name = %s, variable = %d\n", func->name, func->local_variable_count);
    }


    
    fclose(fin);
    delete_visitor(visitor);
    delete_mean_visitor(mvisitor);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    
    return 0;
}