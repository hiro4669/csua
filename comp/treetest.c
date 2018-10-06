#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"
#include "../memory/MEM.h"

int main(void) {
    Expression* left   = cs_create_int_expression(10);
    Expression* right = cs_create_int_expression(20);
    
    Expression* left2   = cs_create_int_expression(100);
    Expression* right2 = cs_create_int_expression(200);
    
    Expression* mul_expr = cs_create_binary_expression(MUL_EXPRESSION, left2, right2);
    
    Expression* add_expr = cs_create_binary_expression(ADD_EXPRESSION, left, mul_expr);
    printf("treetest\n");
    
    Visitor* visitor = create_treeview_visitor();
    traverse_expr(add_expr, visitor);
    
    delete_visitor(visitor);
    
    delete_storage();
    MEM_dump_memory();
    
    
    return 0;
}

