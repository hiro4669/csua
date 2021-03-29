#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"

static int depth = 0;

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
    print_depth();
    fprintf(stderr, "enter castexpr\n");
    increment();
}

static void leave_castexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();    
    fprintf(stderr, "leave castexpr\n");
}


static void enter_boolexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter boolexpr : %d\n", expr->u.boolean_value);
    increment();
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave boolexpr\n");
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}


static void enter_intexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter intexpr : %d\n", expr->u.int_value);
    increment();
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave intexpr\n");

    expr->type = cs_create_type_specifier(CS_INT_TYPE);
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter doubleexpr : %f\n", expr->u.double_value);
    increment();
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave doubleexpr\n");
    expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter identifierexpr : %s\n", expr->u.identifier.name);
    increment();
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave identifierexpr\n");
        
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


static void enter_addexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter addexpr : +\n");
    increment();
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave addexpr\n");
}

static void enter_subexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter subexpr : -\n");
    increment();
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave subexpr\n");
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter mulexpr : *\n");
    increment();
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave mulexpr\n");
}

static void enter_divexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter divexpr : /\n");
    increment();
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave divexpr\n");
}

static void enter_modexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter modexpr : mod \n");
    increment();
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave modexpr\n");
}


static void enter_gtexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter gtexpr : > \n");
    increment();
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave gtexpr\n");
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter geexpr : >= \n");
    increment();
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave geexpr\n");
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter ltexpr : < \n");
    increment();
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave ltexpr\n");
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter leexpr : <= \n");
    increment();
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave leexpr\n");
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter eqexpr : == \n");
    increment();
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave eqexpr\n");
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter neexpr : != \n");
    increment();
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave neexpr\n");
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter landexpr : && \n");
    increment();
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave landexpr\n");
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter lorexpr : || \n");
    increment();
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave lorexpr\n");
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
    print_depth();
    fprintf(stderr, "enter incexpr : ++ \n");
    increment();   
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave incexpr\n");
    incdec_typecheck(expr, visitor);
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter decexpr : -- \n");
    increment();   
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave decexpr\n");
    incdec_typecheck(expr, visitor);
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter minusexpr : - \n");
    increment();   
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave minusexpr\n");

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
    print_depth();
    fprintf(stderr, "enter lognotexpr : ! \n");
    increment();  
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave lognotexpr\n");
}

static Expression* assignment_type_check(TypeSpecifier *ltype, Expression *expr, Visitor *visitor) {
    printf("assignment_type_check\n");
    if (ltype == NULL) {
        fprintf(stderr, "ltype is NULL\n");
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
        printf("double to int\n");
        Expression *cast_expr = cs_create_cast_expression(CS_DOUBLE_TO_INT, expr);
        cast_expr->type = cs_create_type_specifier(CS_INT_TYPE);
        return cast_expr;
    } else if (ltype->basic_type == CS_DOUBLE_TYPE && expr->type->basic_type == CS_INT_TYPE) {
        printf("int to double\n");
        Expression *cast_expr = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast_expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return cast_expr;
    } else {
        fprintf(stderr, "cast error\n");
        exit(1);
    }

    return expr;
}

static void enter_assignexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter assignexpr : %d \n", expr->u.assignment_expression.aope);
    increment(); 
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave assignexpr\n");
    Expression* left = expr->u.assignment_expression.left;
    Expression* right = expr->u.assignment_expression.right;

    expr->u.assignment_expression.right = assignment_type_check(left->type, right, visitor);
    expr->type = left->type;
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter function call :\n");
    increment(); 
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave function call\n");
    expr->type = expr->u.function_call_expression.function->type;
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter exprstmt :\n");
    increment(); 
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave exprstmt\n");
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter declstmt name=%s, type=%d:\n", 
            stmt->u.declaration_s->name,
            stmt->u.declaration_s->type->basic_type);

    MeanVisitor* mvisitor = (MeanVisitor*)visitor;
    CS_Compiler* compiler = mvisitor->compiler;
    if (mvisitor->block) {
        // Todo
    } else {        
        compiler->decl_list = cs_chain_declaration(compiler->decl_list, stmt->u.declaration_s);
    }


            
    increment(); 
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave declstmt\n");
}

static void enter_whilestmt(Statement* stmt, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter whilestatement\n");
    increment();
}

static void leave_whilestmt(Statement* stmt, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave whilestatement\n");
}


static void enter_func(FunctionDefinition* func, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter function:");
    fprintf(stderr, "name(%s), type(%d) ", func->name, func->type->basic_type);
    if (func->parameter) {
        fprintf(stderr, "args = ");
        ParameterList* param = func->parameter;
        while(param) {
            fprintf(stderr, "type(%d), name(%s), ", param->type->basic_type, param->name);
            param = param->next;
        }
        fprintf(stderr, "\n");
    }

    if (!func->block) fprintf(stderr, "\n");
    increment();
}

static void leave_func(FunctionDefinition* func, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave function\n");
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
    

    ((Visitor*)visitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)visitor)->leave_expr_list = leave_expr_list;
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

