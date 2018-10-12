#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"


int main(void) {
    
    FILE *fin = fopen("tests/prog1.cs", "r");
    CS_Compiler* compiler = CS_create_compiler();
    CS_compile(compiler, fin);
    
    Visitor* visitor = create_treeview_visitor();
    Visitor* mean_visitor = (Visitor*)create_mean_visitor();

    printf("--------------\n");
    
    StatementList* stmt_list = compiler->stmt_list;
    while(stmt_list) {
//        printf("type = %d\n", stmt_list->stmt->type);
        traverse_stmt(stmt_list->stmt, mean_visitor);
        stmt_list = stmt_list->next;
    }
   
    printf("--------------\n");
    stmt_list = compiler->stmt_list;
    while(stmt_list) {
        traverse_stmt(stmt_list->stmt, visitor);
        stmt_list = stmt_list->next;
    }
    
    
    /*
    DeclarationList* decl_list = NULL;
    for (decl_list = compiler->decl_list; decl_list; decl_list = decl_list->next) {
        printf("name = %s, type = %d\n", decl_list->decl->name, decl_list->decl->type->basic_type);
    }
    */
    
    
    
    
    fclose(fin);
    delete_visitor(visitor);    
    delete_visitor(mean_visitor);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}