
#include <stdio.h>
#include <stdlib.h>

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



static void enter_castexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter castexpr\n");
}
static void leave_castexpr(Expression* expr, Visitor* visitor) { 
    fprintf(stderr, "leave castexpr\n");
}

static void enter_boolexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter boolexpr : %d\n", expr->u.boolean_value);
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave boolexpr\n");    
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}


static void enter_intexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter intexpr : %d\n", expr->u.int_value);    
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave intexpr\n");
    expr->type = cs_create_type_specifier(CS_INT_TYPE);
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter doubleexpr : %f\n", expr->u.double_value);
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave doubleexpr\n");            
    expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter identifierexpr\n");
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave identifierexpr %s\n", expr->u.identifier.name);
    Declaration* decl = cs_search_decl_in_block();
    if (!decl) {
        decl = cs_search_decl_global(expr->u.identifier.name);
    }
    
    if (decl) {
        expr->type = decl->type;
    } else {
        fprintf(stderr, "Cannot find lefthand type in meanvisitor\n");
        exit(1);
    }
}

static void cast_arithmetic_binary_expr(Expression* expr) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    
    if (cs_is_int(left->type) && cs_is_int(right->type)) {
        return;
    } else if(cs_is_int(left->type) && cs_is_double(right->type)) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        cast->u.cast_expression.expr = left;
        expr->u.binary_expression.left = cast;
    } else if (cs_is_double(left->type) && cs_is_int(right->type)) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        cast->u.cast_expression.expr = right;
        expr->u.binary_expression.right = cast;
    } else if(cs_is_double(left->type) && cs_is_double(right->type)) {
        return;
    } else {
        fprintf(stderr, "type mismatch of cast_arithmetic_binary_expr in meanvisitor\n");
        exit(1);        
    }        
    
}

/* arithmetic calculation*/
static void enter_addexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter addexpr : +\n");
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave addexpr\n");
    cast_arithmetic_binary_expr(expr);
}
static void enter_subexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter subexpr : -\n");
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave subexpr\n");
    cast_arithmetic_binary_expr(expr);
}
static void enter_mulexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter mulexpr : *\n");
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave mulexpr\n");
    cast_arithmetic_binary_expr(expr);    
}
static void enter_divexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter divexpr : /\n");
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave divexpr\n");
    cast_arithmetic_binary_expr(expr);    
}
static void enter_modexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter modexpr : mod \n");
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave modexpr\n");
    cast_arithmetic_binary_expr(expr);    
}


static void enter_gtexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter gtexpr : > \n");
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave gtexpr\n");
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter geexpr : >= \n");
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave geexpr\n");
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter ltexpr : < \n");
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave ltexpr\n");
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter leexpr : <= \n");
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave leexpr\n");
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter eqexpr : == \n");
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave eqexpr\n");
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter neexpr : != \n");
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave neexpr\n");
}


static void logical_type_check(Expression* expr) {
    Expression* left  = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    if (cs_is_boolean(left->type) && cs_is_boolean(right->type)) {
        expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
    } else {
        fprintf(stderr, "type mismatch of compare_type_check in meanvisitor\n");
        exit(1);
    }
}


static void enter_landexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter landexpr : && \n");
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave landexpr\n");
    logical_type_check(expr);
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter lorexpr : || \n");
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave lorexpr\n");
    logical_type_check(expr);
}


static void incdec_typecheck(Expression* expr) {
    Expression* idexpr = expr->u.inc_dec;
    if (idexpr->type->basic_type != CS_INT_TYPE) {
        fprintf(stderr, "type error in incdec_typecheck type=%d\n", idexpr->type->basic_type);
        exit(1);
    }
    expr->type = idexpr->type;
}
static void enter_incexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter incexpr : ++ \n");
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave incexpr\n");
    incdec_typecheck(expr);   
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter decexpr : -- \n");
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave decexpr\n");
    incdec_typecheck(expr);    
}


static void enter_minusexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter minusexpr : - \n");
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave minusexpr\n");   
    TypeSpecifier* type = expr->u.minus_expression->type;
    if ((type->basic_type != CS_INT_TYPE) || (type->basic_type != CS_DOUBLE_TYPE)) {
        fprintf(stderr, "type error in incdec_typecheck type=%d\n", type->basic_type);
    }
    expr->type = type;    
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter lognotexpr : ! \n");
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave lognotexpr\n");
    TypeSpecifier* type = expr->u.logical_not_expression->type;
    if (type->basic_type != CS_BOOLEAN_TYPE) {
        fprintf(stderr, "type error in logical not_typecheck type=%d\n", type->basic_type);
    }
    expr->type = type;    
}




static Expression* assignment_type_check(TypeSpecifier* ltype, Expression* expr) {
    if (ltype->basic_type == expr->type->basic_type) {
        return expr;
    } else if ((ltype->basic_type == CS_INT_TYPE) && (expr->type->basic_type == CS_DOUBLE_TYPE) ) {
        Expression* cast = cs_create_cast_expression(CS_DOUBLE_TO_INT, expr);
        cast->type = cs_create_type_specifier(CS_INT_TYPE);
        return cast;
    } else if ((ltype->basic_type == CS_DOUBLE_TYPE) && (expr->type->basic_type == CS_INT_TYPE) ) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);       
        return cast;
    } else {
        fprintf(stderr, "Type Error\n");
        exit(1);
    }
    return expr;
}
static void enter_assignexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter assignexpr : %d \n", expr->u.assignment_expression.aope);
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave assignexpr\n");
    Expression* left  = expr->u.assignment_expression.left;
    Expression* right = expr->u.assignment_expression.right;
     
    expr->u.assignment_expression.right = assignment_type_check(left->type, right);
    expr->type = left->type;
    
    
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "enter function call :\n");
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    fprintf(stderr, "leave function call\n");
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
    fprintf(stderr, "enter exprstmt :\n");
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    fprintf(stderr, "leave exprstmt\n");
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
    CS_Compiler* compiler = ((MeanVisitor*)visitor)->compiler;
    compiler->decl_list = cs_chain_declaration(compiler->decl_list, stmt->u.declaration_s);
    fprintf(stderr, "enter declstmt\n");    
    
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
    fprintf(stderr, "leave declstmt\n");
    Declaration* decl = stmt->u.declaration_s;
    if (decl->initializer != NULL) {
        decl->initializer = assignment_type_check(decl->type, decl->initializer);
    }
}


MeanVisitor* create_mean_visitor() {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
    
    MeanVisitor* visitor = MEM_malloc(sizeof(MeanVisitor));
    visitor->compiler = cs_get_current_compiler();
    if (visitor->compiler == NULL) {
        fprintf(stderr, "Compile is NULL\n");
        exit(1);
    }
    
    
    
    enter_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);    
    
    

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
    

    ((Visitor*)visitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)visitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)visitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)visitor)->leave_stmt_list = leave_stmt_list;
            
    
    

    return visitor;
}