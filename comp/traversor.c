
#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"

static void traverse_expr_children(Expression* expr, Visitor *visitor);

void traverse_expr(Expression* expr, Visitor* visitor) {
    printf("traverse_expr\n");
    if (expr) {
        if (visitor->enter_expr_list[expr->kind] == NULL) {
            fprintf(stderr, "enter->type(%d) it null\n", expr->kind);
            exit(1);
        }

        visitor->enter_expr_list[expr->kind](expr);
        traverse_expr_children(expr, visitor);
        visitor->leave_expr_list[expr->kind](expr);

    }    
}

static void traverse_expr_children(Expression* expr, Visitor *visitor) {
    switch(expr->kind) {
        case INT_EXPRESSION: {
            break;
        }
        default:
            fprintf(stderr, "No such expr->kind %d\n", expr->kind);
    }
}