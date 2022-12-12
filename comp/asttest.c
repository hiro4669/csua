#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"


int main(void) {
    
    FILE *fin = fopen("tests/print.cs", "r");
    CS_Compiler* compiler = CS_create_compiler();
    CS_compile(compiler, fin);
    
    Visitor* visitor = create_treeview_visitor();
    
    StatementList* stmt_list = compiler->stmt_list;
    printf("--------------\n");
    stmt_list = compiler->stmt_list;
    while(stmt_list) {
        traverse_stmt(stmt_list->stmt, visitor);
        stmt_list = stmt_list->next;
    }       
    
    fclose(fin);
    delete_visitor(visitor);    
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}