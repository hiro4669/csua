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

    printf("--------------\n");
    ExpressionList* expr_list = compiler->expr_list;
    while(expr_list) {
        printf("kind = %d\n", expr_list->expression->kind);
        traverse_expr(expr_list->expression, visitor);
//        traverse_expr(expr_list->expression, visitor);
        
        expr_list = expr_list->next;
    }
    
    
    
    fclose(fin);
    delete_visitor(visitor);    
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}