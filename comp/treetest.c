#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"
#include "../memory/MEM.h"

int main(void) {
    Expression* expr = cs_create_int_expression(10);
    printf("treetest\n");
    
    Visitor* visitor = create_treeview_visitor();
    traverse_expr(expr, visitor);
    
    delete_visitor(visitor);
    
    delete_storage();
    MEM_dump_memory();
    
    
    return 0;
}

