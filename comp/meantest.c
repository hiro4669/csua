#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"


int main(void) {
    
    FILE *fin = fopen("tests/prog2.cs", "r");
    CS_Compiler* compiler = CS_create_compiler();
    CS_compile(compiler, fin);
    
    Visitor* visitor = create_treeview_visitor();
    MeanVisitor* mean_visitor = create_mean_visitor();

    printf("--------------\n");
    FunctionDeclarationList* func_list = compiler->func_list;
    for (; func_list; func_list = func_list->next) {
        printf("func name = %s\n", func_list->func->name);
    }
    
    StatementList* stmt_list = compiler->stmt_list;
    while(stmt_list) {
//        printf("type = %d\n", stmt_list->stmt->type);
        traverse_stmt(stmt_list->stmt, (Visitor*)mean_visitor);
        stmt_list = stmt_list->next;
    }
    printf("");
    if (mean_visitor->check_log != NULL) {
        printf("Log!!!");
        show_mean_error(mean_visitor);
    }

    
   
    printf("--------------\n");
    stmt_list = compiler->stmt_list;
    while(stmt_list) {
        traverse_stmt(stmt_list->stmt, visitor);
        stmt_list = stmt_list->next;
    }
           
    
    fclose(fin);
    delete_visitor(visitor);    
    delete_visitor((Visitor*)mean_visitor);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}