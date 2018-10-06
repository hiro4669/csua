
#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"

static void traverse_expr_children(Expression* expr, Visitor *visitor);

void traverse_expr(Expression* expr, Visitor* visitor) {
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
        case LOGICAL_AND_EXPRESSION:
        case LOGICAL_NOT_EXPRESSION:            
        case LT_EXPRESSION:
        case LE_EXPRESSION:
        case GT_EXPRESSION:
        case GE_EXPRESSION:
        case EQ_EXPRESSION:
        case NE_EXPRESSION:            
        case MOD_EXPRESSION:
        case DIV_EXPRESSION:
        case MUL_EXPRESSION:
        case SUB_EXPRESSION:
        case ADD_EXPRESSION: {
            if (expr->u.binary_expression.left) {
                traverse_expr(expr->u.binary_expression.left, visitor);
            }
            if (expr->u.binary_expression.right) {
                traverse_expr(expr->u.binary_expression.right, visitor);
            }
            break;
        }
        default:
            fprintf(stderr, "No such expr->kind %d\n", expr->kind);
    }
}