#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"


#define cs_is_type(type, cs_type) \
  ((type)->basic_type == (cs_type))

#define cs_is_int(type) \
    (cs_is_type(type, CS_INT_TYPE))
#define cs_is_double(type) \
    (cs_is_type(type, CS_DOUBLE_TYPE))
#define cs_is_boolean(type) \
    (cs_is_type(type, CS_BOOLEAN_TYPE))

#define cs_same_type(type1, type2) \
    ((type1)->basic_type == (type2)->basic_type)

static int depth = 0;

/*
static void increment() {
    depth++;
}

static void decrement() {
    depth--;
}

static void print_depth() {
    for (int i = 0; i < depth; ++i) {
        fprintf(stderr, "  ");
    }
}
*/

static const char* get_type_name(CS_BasicType type) {
    switch(type) {
        case CS_BOOLEAN_TYPE: {
            return "boolean";            
        }
        case CS_INT_TYPE: {
            return "int";
        }
        case CS_DOUBLE_TYPE: {
            return "double";            
        }
        default: {
            return "untyped";
        }
    }
}


static void add_check_log(const char *str, MeanVisitor* mvisitor) {
    MeanCheckLog* log = (MeanCheckLog*)cs_malloc(sizeof(MeanCheckLog));
    log->next = NULL;
    log->log_str = (char*)cs_malloc(strlen(str) + 1);
    strcpy(log->log_str, str);
    MeanCheckLog *p;
    if (mvisitor->mean_log == NULL) {
        mvisitor->mean_log = log;       
        return;
    } else {        
        for (p = mvisitor->mean_log; p->next; p = p->next);
        p->next = log;
    }
}

void show_mean_error(MeanVisitor *mvisitor) {
    MeanCheckLog *p;
    for (p = mvisitor->mean_log; p; p = p->next) {
        fprintf(stderr, "%s\n", p->log_str);
    }
}

static void enter_castexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}

static void leave_castexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
}


static void enter_boolexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}


static void enter_intexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();
    */
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    expr->type = cs_create_type_specifier(CS_INT_TYPE);
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
        
    Declaration *decl = cs_search_declaration(expr->u.identifier.name, ((MeanVisitor*)visitor)->block);
    if (decl) {
        expr->type = decl->type;
        expr->u.identifier.u.decl = decl;
        expr->u.identifier.is_function = CS_FALSE;
        return;
    }

    FunctionDefinition *func = cs_search_function(expr->u.identifier.name);
    if (func) {
        expr->type = func->type;
        expr->u.identifier.u.func = func;
        expr->u.identifier.is_function = CS_TRUE;
        return;
    }
    
    
    fprintf(stderr, "Cannot find identifier type %s\n", expr->u.identifier.name);
    char messages[50];
    sprintf(messages, "%d: Cannot find identifier %s", expr->line_number, expr->u.identifier.name);
    add_check_log(messages, (MeanVisitor*)visitor);    
}


/* Arithmetic expressions */

static CS_Boolean check_nulltype_binary_expression(Expression* expr, Visitor* visitor) {
    CS_Boolean result = CS_TRUE;

    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    if (left->type == NULL) {
        char messages[100];
        sprintf(messages, "%d: Cannot find left expr type", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);
        result = CS_FALSE;
    }

    if (right->type == NULL) {
        char messages[100];
        sprintf(messages, "%d: Cannot find right expr type", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);
        result = CS_FALSE;
    }
    return result;
}

static void check_arithmetic_binary_expression(Expression* expr, Visitor* visitor) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    

    if (!check_nulltype_binary_expression(expr, visitor)) {
        return;
    }

    if (cs_is_int(left->type) && cs_is_int(right->type)) {
        expr->type = cs_create_type_specifier(CS_INT_TYPE);        
        return;
    }

    if (cs_is_int(left->type) && cs_is_double(right->type)) {
        Expression *cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, left);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.left = cast_expr;
        expr->type = expr->u.binary_expression.left->type;        
        return;
    }

    if (cs_is_double(left->type) && cs_is_int(right->type)) {    
        Expression *cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, right);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.right = cast_expr;
        expr->type = left->type;
        return;
    }

    if (cs_is_double(left->type) && cs_is_double(right->type)) {
        expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return;
    }

    /* otherwise error */
    char messages[50];
    sprintf(messages, "%d: %s and %s cannot be calculated as arithmetic expressions\n", 
            expr->line_number, 
            get_type_name(left->type->basic_type), 
            get_type_name(right->type->basic_type));
    add_check_log(messages, (MeanVisitor*)visitor);

    return;
}

static void enter_addexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    check_arithmetic_binary_expression(expr, visitor);
}

static void enter_subexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();
    */
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    check_arithmetic_binary_expression(expr, visitor);
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    check_arithmetic_binary_expression(expr, visitor);
}

static void enter_divexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_arithmetic_binary_expression(expr, visitor);
}

static void enter_modexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_arithmetic_binary_expression(expr, visitor);
}


/* Logical expressions */

static void unacceptable_type_binary_expr(Expression* expr, Visitor* visitor) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    char message[100];
    sprintf(message, "%d: type mismatch in binary expression left:%s, right:%s", 
            expr->line_number, 
            get_type_name(left->type->basic_type),
            get_type_name(right->type->basic_type));
    
    add_check_log(message, (MeanVisitor*)visitor);
}

static void check_compare_expression(Expression* expr, Visitor* visitor) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;

    if (!check_nulltype_binary_expression(expr, visitor)) {
        return;
    }

    if (cs_same_type(left->type, right->type)) {
        //expr->type = cs_create_type_specifier(left->type->basic_type);
        expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
        return;
    }
    if (cs_is_int(left->type) && cs_is_double(right->type)) {
        Expression* cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, left);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.left = cast_expr;
        expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
        return;
    }

    if (cs_is_double(left->type) && cs_is_int(right->type)) {
        Expression* cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, left);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.right = cast_expr;
        expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
        return;
    }
    unacceptable_type_binary_expr(expr, visitor);   
}

static void enter_gtexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    check_compare_expression(expr, visitor);
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_compare_expression(expr, visitor);
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_compare_expression(expr, visitor);
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_compare_expression(expr, visitor);
}

static void add_type_mismatch(CS_BasicType ltype, CS_BasicType rtype, int line_number, Visitor* visitor) {
    char messages[100];
    sprintf(messages, "%d: type mismatched for equality left:%s, right:%s",
        line_number,
        get_type_name(ltype),
        get_type_name(rtype));
    add_check_log(messages, (MeanVisitor*)visitor);
}

static void check_equality_binary_expression(Expression* expr, Visitor* visitor) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;

    if (cs_is_boolean(left->type) && !cs_is_boolean(right->type)) {
        add_type_mismatch(left->type->basic_type, right->type->basic_type, expr->line_number, visitor);       
    } else if (!cs_is_boolean(left->type) && cs_is_boolean(right->type)) {
        add_type_mismatch(left->type->basic_type, right->type->basic_type, expr->line_number, visitor);
    } else {
        check_compare_expression(expr, visitor);
    }
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();
    */
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_equality_binary_expression(expr, visitor);

}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();
    */
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_equality_binary_expression(expr, visitor);
}


static void check_logical_and_or(Expression* expr, Visitor* visitor) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;    

    if (!cs_is_boolean(left->type) || !cs_is_boolean(right->type)) {
        char messages[100];
        sprintf(messages, "%d: type mismatched for logical and/or left:%s, right:%s", 
            expr->line_number,
            get_type_name(left->type->basic_type),
            get_type_name(right->type->basic_type));

        add_check_log(messages, (MeanVisitor*)visitor);
    }
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();    
    */
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_logical_and_or(expr, visitor);
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();
    */
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */
    check_logical_and_or(expr, visitor);
}

static void incdec_typecheck(Expression* expr, Visitor* visitor) {
    Expression* incdec_expr = expr->u.inc_dec;
    char messages[50];
    if (incdec_expr->type == NULL) {
        sprintf(messages, "%d: Cannot find type for inc/dec", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);
        return;
    }

    if (incdec_expr->type->basic_type != CS_INT_TYPE) {
        sprintf(messages, "%d: Type cannot be allowed for ++ or -- %s",
            expr->line_number,
            get_type_name(incdec_expr->type->basic_type));

        add_check_log(messages, (MeanVisitor*)visitor);
        return; 
    }
    
    expr->type = incdec_expr->type;
}

static void enter_incexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();   
    */
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    incdec_typecheck(expr, visitor);
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();   
    */
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    incdec_typecheck(expr, visitor);
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();    
    increment();   
    */
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */

    if (expr->u.minus_expression->type->basic_type != CS_INT_TYPE &&
        expr->u.minus_expression->type->basic_type != CS_DOUBLE_TYPE) {
        char messages[50];
        sprintf(messages, "%d: %s is not int or double\n", 
            expr->line_number,
            get_type_name(expr->u.minus_expression->type->basic_type));

        add_check_log(messages, (MeanVisitor*)visitor);
        return;
    }

    expr->type = expr->u.minus_expression->type;
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment();  
    */
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();    
    */

    if (expr->u.logical_not_expression->type->basic_type != CS_BOOLEAN_TYPE) {
        char messages[50];
        sprintf(messages, "%d: %s is not in\n", 
            expr->line_number,
            get_type_name(expr->u.logical_not_expression->type->basic_type));

        add_check_log(messages, (MeanVisitor*)visitor);
        return;
    }

    expr->type = expr->u.logical_not_expression->type;
    return;
}

static Expression* assignment_type_check(TypeSpecifier *ltype, Expression *expr, Visitor *visitor) {
    if (ltype == NULL) {
        char messages[50];        
        sprintf(messages, "%d: ltype is NULL", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);
        return expr;
    }

    if (expr->type == NULL) {
        fprintf(stderr, "rtype is NULL\n");        
        char messages[50];        
        sprintf(messages, "%d: rtype is NULL", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);         
        return expr;
    }

    if (ltype->basic_type == expr->type->basic_type) {
        return expr;
    } else if (ltype->basic_type == CS_INT_TYPE && expr->type->basic_type == CS_DOUBLE_TYPE) {
        Expression *cast_expr = cs_create_cast_expression(CS_DOUBLE_TO_INT, expr);
        cast_expr->type = cs_create_type_specifier(CS_INT_TYPE);
        return cast_expr;
    } else if (ltype->basic_type == CS_DOUBLE_TYPE && expr->type->basic_type == CS_INT_TYPE) {
        Expression *cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return cast_expr;
    } else {        
        char messages[100];
        sprintf(messages, "%d: cast mismatch left = %s, right = %s",
        expr->line_number,
        get_type_name(ltype->basic_type),
        get_type_name(expr->type->basic_type));
        add_check_log(messages, (MeanVisitor*)visitor);     

        //exit(1);
    }

    return expr;
}

static void enter_assignexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment(); 
    */
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    Expression* left = expr->u.assignment_expression.left;
    Expression* right = expr->u.assignment_expression.right;

    expr->u.assignment_expression.right = assignment_type_check(left->type, right, visitor);
    expr->type = left->type;
}

static Expression* check_argument(TypeSpecifier* def_type, Expression* arg, Visitor* visitor) {
    if (def_type->basic_type == arg->type->basic_type) {
        // nothing to do
        fprintf(stderr, "nothing to do\n");
        return arg;
    }

    if (cs_is_int(def_type) && cs_is_double(arg->type)) {
        Expression* cast_expr = cs_create_cast_expression(CS_DOUBLE_TO_INT, arg);
        cast_expr->type = cs_create_type_specifier(CS_INT_TYPE);
        return cast_expr;        
    }
    if (cs_is_double(def_type) && cs_is_int(arg->type)) {
        Expression* cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, arg);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return cast_expr;
    }

    // error
    char messages[100];
    sprintf(messages, "%d: cast mismatch left = %s, right = %s",
            arg->line_number,
            get_type_name(def_type->basic_type),
            get_type_name(arg->type->basic_type));
    add_check_log(messages, (MeanVisitor*)visitor);


    return arg;
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
    /*
    print_depth();
    increment(); 
    */
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    expr->type = expr->u.function_call_expression.function->type;

    fprintf(stderr, "type = %d\n", expr->u.function_call_expression.function->kind);

    if (expr->u.function_call_expression.function->kind == IDENTIFIER_EXPRESSION) {
        Expression* ident_expr = expr->u.function_call_expression.function;
        FunctionDefinition* func = ident_expr->u.identifier.u.func;
        fprintf(stderr, "func name = %s\n", func->name);
        ParameterList* pos;
        ArgumentList* arg_pos;
        int param_count = 0;
        for (pos = func->parameter; pos; pos = pos->next) {        
            fprintf(stderr, "name = %s, type=%d \n", pos->name, pos->type->basic_type);
            param_count++;
        }
        int arg_count = 0;
        for (arg_pos = expr->u.function_call_expression.args; arg_pos; arg_pos = arg_pos->next) {
            fprintf(stderr, "expr type = %d\n", arg_pos->expression->type->basic_type);
            arg_count++;
        }

        if (param_count != arg_count) {
            char messages[50];
            sprintf(messages, "%d: The number of arguments does not match", expr->line_number);
            add_check_log(messages, (MeanVisitor*)visitor);
            return;
        }
        int i;
        for (i = 0, pos = func->parameter, 
            arg_pos = expr->u.function_call_expression.args; i < param_count; ++i) {
                arg_pos->expression = check_argument(pos->type, arg_pos->expression, visitor);

            pos = pos->next;
            arg_pos = arg_pos->next;
        }
    } else {
        char messages[50];
        sprintf(messages, "%d: The function cannot be invoked", expr->line_number);
        add_check_log(messages, (MeanVisitor*)visitor);
        return;

    }
}

/* For statement */
static void add_decl_to_function(FunctionDefinition* func, Declaration* decl) {
    func->local_variable = MEM_realloc(func->local_variable, sizeof(Declaration*) * 
                            (func->local_variable_count + 1));
    func->local_variable[func->local_variable_count] = decl;
    decl->index = func->local_variable_count;
    func->local_variable_count++;
    //fprintf(stderr, "-------- local val count = %s: %d\n", decl->name, func->local_variable_count);
}


static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
    /*
    print_depth();
    //fprintf(stderr, "enter exprstmt :\n");
    increment(); 
    */
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    //fprintf(stderr, "leave exprstmt\n");
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
    //print_depth();
    /*
    fprintf(stderr, "enter declstmt name=%s, type=%d:\n", 
            stmt->u.declaration_s->name,
            stmt->u.declaration_s->type->basic_type);
    */

    MeanVisitor* mvisitor = (MeanVisitor*)visitor;
    CS_Compiler* compiler = mvisitor->compiler;

    if (mvisitor->block) {
        //fprintf(stderr, "add decl in a block\n");
        mvisitor->block->declaration_list = cs_chain_declaration(mvisitor->block->declaration_list,
            stmt->u.declaration_s); 
        // kokokara
        //fprintf(stderr, "block type = %d\n", mvisitor->block->type);

        if (mvisitor->block->type == FUNCTION_BLOCK) {
            fprintf(stderr, "add decl from declstmt\n");
            add_decl_to_function(mvisitor->block->parent.function.function, stmt->u.declaration_s);
        } else {
            char messages[100];
            sprintf(messages, "%d: Cannot declare in this block\n", stmt->line_number);
            add_check_log(messages, (MeanVisitor*)visitor);
        }
        


    } else {
        //fprintf(stderr, "add decl as global\n");
        compiler->decl_list = cs_chain_declaration(compiler->decl_list, stmt->u.declaration_s);
    }


            
    //increment(); 
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    */
    //fprintf(stderr, "leave declstmt\n");
}

static void enter_whilestmt(Statement* stmt, Visitor* visitor) {
    /*
    print_depth();
    //fprintf(stderr, "enter whilestatement\n");
    increment();
    */

    MeanVisitor* mvisitor = (MeanVisitor*)visitor;
    Block* block = stmt->u.while_s.block;
    if (block) {
        mvisitor->block = block;
    }
    
}

static void leave_whilestmt(Statement* stmt, Visitor* visitor) {    
    MeanVisitor* mvisitor = (MeanVisitor*)visitor;
    Block* block = stmt->u.while_s.block;
    if (block) {
        mvisitor->block = block->outer_block;
    }    
}

static void enter_returnstmt(Statement* stmt, Visitor* visitor) {
    fprintf(stderr, "enter returnstmt\n");
}
static void leave_returnstmt(Statement* stmt, Visitor* visitor) {
    fprintf(stderr, "leave returnstmt\n");

    Expression* ret_expr = stmt->u.return_s.return_expr;
    FunctionDefinition* func = ((MeanVisitor*)visitor)->func;
    if (ret_expr == NULL) {
        fprintf(stderr, "return value is null\n");
        if (func) {
            fprintf(stderr, "func exists\n");
            switch(func->type->basic_type) {
                case CS_BOOLEAN_TYPE: {
                    ret_expr = cs_create_boolean_expression(CS_FALSE);
                    break;
                }
                case CS_INT_TYPE: {
                    ret_expr = cs_create_int_expression(0);
                    break;
                }
                case CS_DOUBLE_TYPE: {
                    ret_expr = cs_create_double_expression(0.0);
                    break;
                }
                default: {
                    fprintf(stderr, "undefined type in leave_stmt\n");
                    exit(1);
                }
            }
            stmt->u.return_s.return_expr = ret_expr;
        } else {
            // error
            char messages[100];
            sprintf(messages, "%d: Cannot declare return in global\n", stmt->line_number);
            add_check_log(messages, (MeanVisitor*)visitor);            
        }
    } else {
        fprintf(stderr, "return value is not null\n");

        if (func) {
            if (cs_same_type(ret_expr->type, func->type)) {
                fprintf(stderr, "nothing to do\n");
                return;
            }

            if (cs_is_int(func->type) && cs_is_double(ret_expr->type)) {
                //fprintf(stderr, "cast to int\n");
                Expression* cast_expr = cs_create_cast_expression(CS_DOUBLE_TO_INT, ret_expr);
                cast_expr->type = cs_create_type_specifier(CS_INT_TYPE);
                stmt->u.return_s.return_expr = cast_expr;
                
            } else if (cs_is_double(func->type) && cs_is_int(ret_expr->type)) {
                //fprintf(stderr, "cast to double\n");
                Expression* cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, ret_expr);
                cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
                stmt->u.return_s.return_expr = cast_expr;

            } else {
                //fprintf(stderr, "type mismatch\n");
                char messages[100];
                sprintf(messages, "%d: cast mismatch left = %s, right = %s",
                stmt->line_number,
                get_type_name(func->type->basic_type),
                get_type_name(ret_expr->type->basic_type));
                add_check_log(messages, (MeanVisitor*)visitor);            
            }
        } else {
            // error
            char messages[100];
            sprintf(messages, "%d: Cannot declare return in global\n", stmt->line_number);
            add_check_log(messages, (MeanVisitor*)visitor);     
        }
    }

    //exit(1);
}




static void enter_func(FunctionDefinition* func, Visitor* visitor) {
    /*
    print_depth();
    fprintf(stderr, "enter function:");
    fprintf(stderr, "name(%s), type(%d) ", func->name, func->type->basic_type);
    */
   ((MeanVisitor*)visitor)->func = func;
    if (func->parameter) {
        //fprintf(stderr, "args = ");
        ParameterList* param = func->parameter;
        while(param) {
            //fprintf(stderr, "type(%d), name(%s), ", param->type->basic_type, param->name);            
            if (func->block) {
                Declaration* decl = cs_create_declaration(param->type->basic_type, param->name, NULL);
                func->block->declaration_list = cs_chain_declaration(func->block->declaration_list, decl);
                add_decl_to_function(func, decl);
            }

            param = param->next;
            
            
        }
        //fprintf(stderr, "\n");
    }

    if (func->block) {
        ((MeanVisitor*)visitor)->block = func->block;
    } else {
        //fprintf(stderr, "\n");
    }

    //if (!func->block) fprintf(stderr, "\n");
    //increment();
}

static void leave_func(FunctionDefinition* func, Visitor* visitor) {
    /*
    decrement();
    print_depth();
    fprintf(stderr, "leave function\n");
    */
    if (func->block) {
        ((MeanVisitor*)visitor)->block = func->block->outer_block;
    }
    ((MeanVisitor*)visitor)->func = NULL;
}

static void init_visit_stmt_functions(visit_stmt *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;    
}

static void init_visit_expr_functions(visit_expr *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;
}


MeanVisitor* create_mean_visitor() {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
    
    //Visitor* visitor = MEM_malloc(sizeof(Visitor));
    MeanVisitor* visitor = MEM_malloc(sizeof(MeanVisitor));    
    visitor->compiler = cs_get_current_compiler();
    visitor->block = NULL;
    visitor->mean_log = NULL;
    visitor->func = NULL;

    if (!visitor->compiler) {
        fprintf(stderr, "Compiler is NULL\n");
        exit(1);
    }
    

    enter_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);


    init_visit_expr_functions(enter_expr_list, EXPRESSION_KIND_PLUS_ONE);
    init_visit_expr_functions(leave_expr_list, EXPRESSION_KIND_PLUS_ONE);
    init_visit_stmt_functions(enter_stmt_list, STATEMENT_TYPE_COUNT_PLUS_ONE);
    init_visit_stmt_functions(leave_stmt_list, STATEMENT_TYPE_COUNT_PLUS_ONE);

    

    enter_expr_list[BOOLEAN_EXPRESSION]       = enter_boolexpr;
    enter_expr_list[INT_EXPRESSION]           = enter_intexpr;
    enter_expr_list[DOUBLE_EXPRESSION]        = enter_doubleexpr;
    enter_expr_list[IDENTIFIER_EXPRESSION]    = enter_identexpr;    
    enter_expr_list[ADD_EXPRESSION]           = enter_addexpr;
    enter_expr_list[SUB_EXPRESSION]           = enter_subexpr;
    enter_expr_list[MUL_EXPRESSION]           = enter_mulexpr;
    enter_expr_list[DIV_EXPRESSION]           = enter_divexpr;
    enter_expr_list[MOD_EXPRESSION]           = enter_modexpr;    
    enter_expr_list[GT_EXPRESSION]            = enter_gtexpr;
    enter_expr_list[GE_EXPRESSION]            = enter_geexpr;
    enter_expr_list[LT_EXPRESSION]            = enter_ltexpr;
    enter_expr_list[LE_EXPRESSION]            = enter_leexpr;
    enter_expr_list[EQ_EXPRESSION]            = enter_eqexpr;
    enter_expr_list[NE_EXPRESSION]            = enter_neexpr;
    enter_expr_list[LOGICAL_AND_EXPRESSION]   = enter_landexpr;
    enter_expr_list[LOGICAL_OR_EXPRESSION]    = enter_lorexpr;
    enter_expr_list[INCREMENT_EXPRESSION]     = enter_incexpr;
    enter_expr_list[DECREMENT_EXPRESSION]     = enter_decexpr;
    enter_expr_list[MINUS_EXPRESSION]         = enter_minusexpr;
    enter_expr_list[LOGICAL_NOT_EXPRESSION]   = enter_lognotexpr;
    enter_expr_list[ASSIGN_EXPRESSION]        = enter_assignexpr;
    enter_expr_list[FUNCTION_CALL_EXPRESSION] = enter_funccallexpr;
    enter_expr_list[CAST_EXPRESSION]          = enter_castexpr;
    
    enter_stmt_list[EXPRESSION_STATEMENT]     = enter_exprstmt;
    enter_stmt_list[DECLARATION_STATEMENT]    = enter_declstmt;
    enter_stmt_list[WHILE_STATEMENT]          = enter_whilestmt;
    enter_stmt_list[RETURN_STATEMENT]         = enter_returnstmt;
    
    
    
    leave_expr_list[BOOLEAN_EXPRESSION]       = leave_boolexpr;
    leave_expr_list[INT_EXPRESSION]           = leave_intexpr;
    leave_expr_list[DOUBLE_EXPRESSION]        = leave_doubleexpr;
    leave_expr_list[IDENTIFIER_EXPRESSION]    = leave_identexpr;    
    leave_expr_list[ADD_EXPRESSION]           = leave_addexpr;
    leave_expr_list[SUB_EXPRESSION]           = leave_subexpr;
    leave_expr_list[MUL_EXPRESSION]           = leave_mulexpr;
    leave_expr_list[DIV_EXPRESSION]           = leave_divexpr;
    leave_expr_list[MOD_EXPRESSION]           = leave_modexpr;    
    leave_expr_list[GT_EXPRESSION]            = leave_gtexpr;
    leave_expr_list[GE_EXPRESSION]            = leave_geexpr;
    leave_expr_list[LT_EXPRESSION]            = leave_ltexpr;
    leave_expr_list[LE_EXPRESSION]            = leave_leexpr;
    leave_expr_list[EQ_EXPRESSION]            = leave_eqexpr;
    leave_expr_list[NE_EXPRESSION]            = leave_neexpr;
    leave_expr_list[LOGICAL_AND_EXPRESSION]   = leave_landexpr;
    leave_expr_list[LOGICAL_OR_EXPRESSION]    = leave_lorexpr;
    leave_expr_list[INCREMENT_EXPRESSION]     = leave_incexpr;
    leave_expr_list[DECREMENT_EXPRESSION]     = leave_decexpr;
    leave_expr_list[DECREMENT_EXPRESSION]     = leave_decexpr;
    leave_expr_list[MINUS_EXPRESSION]         = leave_minusexpr;
    leave_expr_list[LOGICAL_NOT_EXPRESSION]   = leave_lognotexpr;
    leave_expr_list[ASSIGN_EXPRESSION]        = leave_assignexpr;
    leave_expr_list[FUNCTION_CALL_EXPRESSION] = leave_funccallexpr;
    leave_expr_list[CAST_EXPRESSION]          = leave_castexpr;
    
    leave_stmt_list[EXPRESSION_STATEMENT]     = leave_exprstmt;
    leave_stmt_list[DECLARATION_STATEMENT]    = leave_declstmt;
    leave_stmt_list[WHILE_STATEMENT]          = leave_whilestmt;
    leave_stmt_list[RETURN_STATEMENT]         = leave_returnstmt;
    

    ((Visitor*)visitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)visitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)visitor)->notify_expr_list = NULL;
    ((Visitor*)visitor)->end_block_func   = NULL;
    ((Visitor*)visitor)->after_cond_func  = NULL;


    ((Visitor*)visitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)visitor)->leave_stmt_list = leave_stmt_list;

    ((Visitor*)visitor)->enter_func = enter_func;
    ((Visitor*)visitor)->leave_func = leave_func;
    
    
    

    return visitor;
}

void delete_mean_visitor(MeanVisitor* visitor) {
    MEM_free(((Visitor*)visitor)->enter_expr_list);
    MEM_free(((Visitor*)visitor)->leave_expr_list);
    MEM_free(((Visitor*)visitor)->enter_stmt_list);
    MEM_free(((Visitor*)visitor)->leave_stmt_list);    
    MEM_free(visitor);
}

