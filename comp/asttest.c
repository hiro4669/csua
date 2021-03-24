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

    
    //Visitor* visitor = create_treeview_visitor();
    MeanVisitor* visitor = create_mean_visitor();

    printf("--------------\n");
    printf("-- Function Definition --\n");
    FunctionDefinition* function = compiler->function_list;
    while (function) {
        traverse_func(function, (Visitor*)visitor);
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
        traverse_stmt(stmt_list->stmt, (Visitor*)visitor);
        stmt_list = stmt_list->next;
    }
    
    
    
    fclose(fin);
    //delete_visitor((Visitor*)visitor);
    delete_mean_visitor(visitor);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}