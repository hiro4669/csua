#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "csua.h"
#include "visitor.h"
#include "../svm/svm.h"
#include "../memory/MEM.h"


static size_t get_opsize(OpcodeInfo* op) {
    size_t size = strlen(op->parameter);
    size *= 2;
    return size;
}

/*
static int add_constant(CS_Executable* exec, CS_ConstantPool* cp) {
    exec->constant_pool = (CS_ConstantPool*)MEM_realloc(exec->constant_pool, exec->constant_pool_count+1);
    exec->constant_pool[exec->constant_pool_count] = *cp;
    return exec->constant_pool_count++;
}
*/

static int add_constant(CS_Executable* exec, CS_ConstantPool* cp) {    
    exec->constant_pool = (CS_ConstantPool*)MEM_realloc(exec->constant_pool, 
        sizeof(CS_ConstantPool) * (exec->constant_pool_count+1));
    exec->constant_pool[exec->constant_pool_count] = *cp;
    return exec->constant_pool_count++;
}





static void gen_byte_code(CodegenVisitor* cvisitor, SVM_Opcode op, ...) {
    va_list ap;
    va_start(ap, op);

    OpcodeInfo oInfo = svm_opcode_info[op];
    /*
    fprintf(stderr, "name  = %s\n", oInfo.opname);
    fprintf(stderr, "param = %s\n", oInfo.parameter);
    */

    
    if ((cvisitor->pos + 1 + get_opsize(&oInfo)) >= cvisitor->current_code_size) {        
        cvisitor->code = MEM_realloc(cvisitor->code, 
            cvisitor->current_code_size += cvisitor->CODE_ALLOC_SIZE);
    }
    

    cvisitor->code[cvisitor->pos++] = op & 0xff;
    for (int i = 0; i < strlen(oInfo.parameter); ++i) {
        switch(oInfo.parameter[i]) {
            case 'i': {// 2byte index
                int operand = va_arg(ap, int);
                cvisitor->code[cvisitor->pos++] = (operand >> 8) & 0xff;
                cvisitor->code[cvisitor->pos++] = (operand >> 0) & 0xff;
                break;
            }
            default: {
                fprintf(stderr, "undefined parameter\n");
                exit(1);
            }
        }
    }
    va_end(ap);    
}

static void enter_castexpr(Expression* expr, Visitor* visitor) {    
}
static void leave_castexpr(Expression* expr, Visitor* visitor) {     
    switch (expr->u.cast_expression.ctype) {
        case CS_INT_TO_DOUBLE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_CAST_INT_TO_DOUBLE);
            break;
        }
        case CS_DOUBLE_TO_INT: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_CAST_DOUBLE_TO_INT);
            break;            
        }
        default: {
            fprintf(stderr, "unknown cast type in castexpr\n");
            exit(1);
        }
    }    
}

static void enter_boolexpr(Expression* expr, Visitor* visitor) {
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {        
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;
    CS_ConstantPool cp;
    cp.type = CS_CONSTANT_INT;
    cp.u.c_int = expr->u.boolean_value;
    int idx = add_constant(cvisitor->exec, &cp);

    gen_byte_code(cvisitor, SVM_PUSH_INT, idx);    
}

static void enter_intexpr(Expression* expr, Visitor* visitor) {
    //fprintf(stderr, "enter intexpr \n");
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {    
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;
    
    CS_ConstantPool cp;
    cp.type = CS_CONSTANT_INT;
    cp.u.c_int = expr->u.int_value;
    
    int idx = add_constant(cvisitor->exec, &cp);
    /*
    fprintf(stderr, "leave intexpr idx = %d\n", idx);
    fprintf(stderr, "            value = %d\n", cp.u.c_int);
    */
    gen_byte_code(cvisitor, SVM_PUSH_INT, idx);    
    
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {    
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {    
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;

    CS_ConstantPool cp;
    cp.type = CS_CONSTANT_DOUBLE;
    cp.u.c_double = expr->u.double_value;
    //fprintf(stderr, "d value = %f\n", cp.u.c_double);
    int idx = add_constant(cvisitor->exec, &cp);
    //fprintf(stderr, "idx = %d\n", idx);

    gen_byte_code(cvisitor, SVM_PUSH_DOUBLE, idx);
}



static void generate_pop_static(CodegenVisitor* cvisitor, TypeSpecifier* type, int  idx) {
    switch (type->basic_type) {
        case CS_BOOLEAN_TYPE:
        case CS_INT_TYPE: {
            gen_byte_code(cvisitor, SVM_POP_STATIC_INT, idx);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code(cvisitor, SVM_POP_STATIC_DOUBLE, idx);
            break;
        }
        default: {
            fprintf(stderr, "undefined type");
            exit(1);
        }
    }
}

static void generate_pop_stack(CodegenVisitor* cvisitor, TypeSpecifier* type, int idx) {
    switch (type->basic_type) {
        case CS_BOOLEAN_TYPE:
        case CS_INT_TYPE: {
            gen_byte_code(cvisitor, SVM_POP_STACK_INT, idx);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code(cvisitor, SVM_POP_STACK_DOUBLE, idx);
            break;
        }
        default: {
            fprintf(stderr, "undefined type");
            exit(1);
        }
    }
}


static void generate_push_static(CodegenVisitor* cvisitor, TypeSpecifier* type, int  idx) {
    switch (type->basic_type) {
        case CS_BOOLEAN_TYPE:
        case CS_INT_TYPE: {
            gen_byte_code(cvisitor, SVM_PUSH_STATIC_INT, idx);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code(cvisitor, SVM_PUSH_STATIC_DOUBLE, idx);
            break;
        }
        default: {
            fprintf(stderr, "undefined type");
            exit(1);
        }
    }
}

static void generate_push_stack(CodegenVisitor* cvisitor, TypeSpecifier* type, int idx) {
    switch (type->basic_type) {
        case CS_BOOLEAN_TYPE:
        case CS_INT_TYPE: {
            gen_byte_code(cvisitor, SVM_PUSH_STACK_INT, idx);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code(cvisitor, SVM_PUSH_STACK_DOUBLE, idx);
            break;
        }
        default: {
            fprintf(stderr, "undefined type");
            exit(1);
        }
    }

}


static void enter_identexpr(Expression* expr, Visitor* visitor) {
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    
    /*
    fprintf(stderr, "type = %d\n", expr->u.identifier.is_function);
    fprintf(stderr, "name = %s\n", expr->u.identifier.name);
    */
    
    CS_Boolean is_function = expr->u.identifier.is_function;

    if (is_function) {        
        FunctionDefinition* func = expr->u.identifier.u.func;
        fprintf(stderr, "func idx = %d\n", func->index);        
        gen_byte_code((CodegenVisitor*)visitor, SVM_PUSH_FUNCTION, func->index);        
        // not yet
    } else {
        Declaration* decl = expr->u.identifier.u.decl;
        //fprintf(stderr, "decl idx = %d\n", decl->index);
        //fprintf(stderr, "expr type= %d\n", expr->type->basic_type);
        CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;
        if (decl->is_local) {
            //fprintf(stderr, "is local\n");
            //fprintf(stderr, "idx = %d\n", decl->index);
            switch (cvisitor->v_state) {
                case VISIT_NORMAL: {
                    generate_push_stack(cvisitor, expr->type, decl->index);
                    break;
                }
                case VISIT_NORMAL_ASSIGN: {
                    generate_pop_stack(cvisitor, expr->type, decl->index);
                    if (cvisitor->assign_depth > 1) {
                        generate_push_stack(cvisitor, expr->type, decl->index);
                    }
                    break;
                }
                default: {
                    fprintf(stderr, "v_state error\n");
                    exit(1);
                }
            }
        } else {            
            switch (cvisitor->v_state) {
                case VISIT_NORMAL: { // generate push
                    generate_push_static(cvisitor, expr->type, decl->index);
                    break;
                }
                case VISIT_NORMAL_ASSIGN: { // generate pop
                    generate_pop_static(cvisitor, expr->type, decl->index);
                    if (cvisitor->assign_depth > 1) {
                        generate_push_static(cvisitor, expr->type, decl->index);
                    }
                    break;
                }
                default: {
                    fprintf(stderr, "v_state error\n");
                    exit(1);
                }
            }                        
        }        
    }
}


static void enter_addexpr(Expression* expr, Visitor* visitor) {
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    switch (expr->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_ADD_INT);
            break;
        }        
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_ADD_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "%d: unknown type \n", expr->line_number);
            break;
        }        
    }
}

static void enter_subexpr(Expression* expr, Visitor* visitor) {
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {    
    switch (expr->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_SUB_INT);
            break;
        }        
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_SUB_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "%d: unknown type \n", expr->line_number);
            break;
        }        
    }    
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    switch (expr->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_MUL_INT);
            break;
        }        
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_MUL_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "%d: unknown type \n", expr->line_number);
            break;
        }        
    }    
}

static void enter_divexpr(Expression* expr, Visitor* visitor) {
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    switch (expr->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_DIV_INT);
            break;
        }        
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_DIV_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "%d: unknown type \n", expr->line_number);
            break;
        }        
    }
}

static void enter_modexpr(Expression* expr, Visitor* visitor) {
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    switch (expr->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_MOD_INT);
            break;
        }        
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_MOD_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "%d: unknown type \n", expr->line_number);
            break;
        }        
    }    
}

static void enter_gtexpr(Expression* expr, Visitor* visitor) {
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_GT_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_GT_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in gtexpr\n");
            exit(1);
        }
    }
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_GE_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_GE_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in geexpr\n");
            exit(1);
        }
    }
    
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {    
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_LT_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_LT_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in ltexpr\n");
            exit(1);
        }
    }
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_LE_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_LE_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in leexpr\n");
            exit(1);
        }
    }
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_EQ_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_EQ_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in eqexpr\n");
            exit(1);
        }
    }    
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {    
    switch (expr->u.binary_expression.left->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_NE_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_NE_DOUBLE);
            break;
        }
        default: {
            fprintf(stderr, "unknown type in neexpr\n");
            exit(1);
        }
    }    
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {    
    gen_byte_code((CodegenVisitor*)visitor, SVM_LOGICAL_AND);
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    gen_byte_code((CodegenVisitor*)visitor, SVM_LOGICAL_OR);
}

static void enter_incexpr(Expression* expr, Visitor* visitor) {
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {    
    switch (expr->u.inc_dec->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_INCREMENT);
            break;
        }        
        default: {
            fprintf(stderr, "unknown type in incexpr\n");
            exit(1);
            break;
        }
    }
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    switch (expr->u.inc_dec->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_DECREMENT);
            break;
        }        
        default: {
            fprintf(stderr, "unknown type in decexpr\n");
            exit(1);
            break;
        }
    }
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {    
    switch (expr->u.minus_expression->type->basic_type) {
        case CS_INT_TYPE: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_MINUS_INT);
            break;
        }
        case CS_DOUBLE_TYPE: {            
            gen_byte_code((CodegenVisitor*)visitor, SVM_MINUS_DOUBLE);
            break;
        }
        default: {
            exit(1);
        }
    }    
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    gen_byte_code((CodegenVisitor*)visitor, SVM_LOGICAL_NOT);
    //fprintf(stderr, "log not expr\n");
    //exit(1);
}

static void enter_assignexpr(Expression* expr, Visitor* visitor) {
    ((CodegenVisitor*)visitor)->assign_depth++;
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    --((CodegenVisitor*)visitor)->assign_depth;
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    //fprintf(stderr, "func call\n");
    gen_byte_code((CodegenVisitor*)visitor, SVM_INVOKE);
    //exit(1);
}

static void notify_assignexpr(Expression* expr, Visitor* visitor) {
    //fprintf(stderr, "notify assign expr\n");
    ((CodegenVisitor*)visitor)->v_state = VISIT_NORMAL_ASSIGN;
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;

    switch (cvisitor->v_state) {
        case VISIT_NORMAL_ASSIGN: {
            cvisitor->v_state = VISIT_NORMAL;
            break;
        }
        case VISIT_NORMAL: {
            gen_byte_code((CodegenVisitor*)visitor, SVM_POP);
            break;            
        }
        default: {
            break;
        }
    }
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
}
static void leave_declstmt(Statement* stmt, Visitor* visitor) {
}

static void enter_whilestmt(Statement* stmt, Visitor* visitor) {
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;
    //fprintf(stderr, "enter while statement\n");
    //fprintf(stderr, "pos = %02x\n", cvisitor->pos);
    int loop_label = get_index(cvisitor->jtable);
    stmt->u.while_s.loop_label = loop_label;
    //fprintf(stderr, "loop_label = %d\n", loop_label);
    //uint16_t address = get_address(cvisitor->jtable, loop_label);
    set_address(cvisitor->jtable, loop_label, cvisitor->pos);
    //uint16_t address = get_address(cvisitor->jtable, loop_label);
    //fprintf(stderr, "address = %02x\n", address);


}
static void leave_whilestmt(Statement* stmt, Visitor* visitor) {
    //fprintf(stderr, "leave while statement\n");
}

static void end_block_func(Statement* stmt, Visitor* visitor, StatementType s_type) {
    //fprintf(stderr, "end block func\n");
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;

    switch (s_type) {
        case WHILE_STATEMENT: {
//            fprintf(stderr, "while statement\n");
            gen_byte_code(cvisitor, SVM_JUMP, stmt->u.while_s.loop_label);
            set_address(cvisitor->jtable, stmt->u.while_s.block->parent.statement.break_label, cvisitor->pos);
            uint16_t address= get_address(cvisitor->jtable, stmt->u.while_s.block->parent.statement.break_label);
//            fprintf(stderr, "address = %02x\n", address);

            break;
        }
        default: {
            fprintf(stderr, "unknown stateement\n");
            exit(1);
        }
    }
}

static void after_cond_func(Statement* stmt, Visitor* visitor, StatementType s_type) {    
    //fprintf(stderr, "after_cond_func\n");
    CodegenVisitor* cvisitor = (CodegenVisitor*)visitor;
    switch (s_type) {
        case WHILE_STATEMENT: {
            //fprintf(stderr, "while statement\n");
            stmt->u.while_s.block->parent.statement.break_label = get_index(cvisitor->jtable);
            //fprintf(stderr, "braek_label = %d\n", stmt->u.while_s.block->parent.statement.break_label);

            //gen_byte_code(cvisitor, SVM_PUSH_STATIC_DOUBLE, idx);
            gen_byte_code(cvisitor, SVM_JUMP_IF_FALSE,
                stmt->u.while_s.block->parent.statement.break_label);

            break;
        }
        default: {
            fprintf(stderr, "unknown stateement\n");
            exit(1);
        }
    }
}

static uint16_t fetch2(uint8_t *code, uint32_t pos) {
    uint8_t first = code[pos];
    uint8_t second = code[pos+1];
    return (uint16_t)(first << 8 | second);
}

void backpatch(CodegenVisitor* cvisitor) {    
    fprintf(stderr, "--- backpatch --\n");
    for (int i = 0; i < cvisitor->pos; ++i) {
        //fprintf(stderr, "%02x ", cvisitor->code[i]);
        switch (cvisitor->code[i]) {
            case SVM_JUMP:
            case SVM_JUMP_IF_TRUE:
            case SVM_JUMP_IF_FALSE: {
                OpcodeInfo oInfo = svm_opcode_info[cvisitor->code[i]];
                //fprintf(stderr, "name = %s\n", oInfo.opname);
                for (int j = 0; j < strlen(oInfo.parameter); j++) {
                    switch(oInfo.parameter[j]) {
                        case 'i': {
                            uint16_t idx = fetch2(cvisitor->code, ++i);
                            //fprintf(stderr, "idx = %d\n", idx);
                            uint16_t address = get_address(cvisitor->jtable, idx);
                            //fprintf(stderr, "address = %02x\n", address);
                            cvisitor->code[i++] = (address >> 8) & 0xff;
                            cvisitor->code[i++] = (address >> 0) & 0xff;
                            break;
                        }
                        default: {
                            fprintf(stderr, "unknown parameter\n");
                            exit(1);
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }


    }
    fprintf(stderr, "\n");




}


static void enter_func(FunctionDefinition* func, Visitor* visitor) {
}
static void leave_func(FunctionDefinition* func, Visitor* visitor) {
}

static void init_visit_stmt_functions(visit_stmt *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;    
}

static void init_visit_expr_functions(visit_expr *func_list, size_t size) {
    for (int i = 0; i < size; ++i) func_list[i] = NULL;
}

CodegenVisitor* create_codegen_visitor(CS_Compiler* compiler, CS_Executable* exec) {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;

    visit_expr* notify_expr_list;

    if (!compiler || !exec) {
        fprintf(stderr, "Compiler or Executable is NULL\n");
        exit(1);
    }

    CodegenVisitor* cvisitor = (CodegenVisitor*)MEM_malloc(sizeof(CodegenVisitor));
    cvisitor->compiler = compiler;
    cvisitor->exec = exec;
    cvisitor->jtable = create_jumptable();
    cvisitor->v_state = VISIT_NORMAL;
    cvisitor->assign_depth = 0;
    cvisitor->CODE_ALLOC_SIZE = 10;
    cvisitor->current_code_size = 0;
    cvisitor->pos = 0;
    cvisitor->code = NULL;



    enter_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) * STATEMENT_TYPE_COUNT_PLUS_ONE);

    notify_expr_list = (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);

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


    notify_expr_list[ASSIGN_EXPRESSION]       = notify_assignexpr;





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

    ((Visitor*)cvisitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)cvisitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)cvisitor)->notify_expr_list = notify_expr_list;

    ((Visitor*)cvisitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)cvisitor)->leave_stmt_list = leave_stmt_list;

    ((Visitor*)cvisitor)->end_block_func  = end_block_func;
    ((Visitor*)cvisitor)->after_cond_func = after_cond_func;


    ((Visitor*)cvisitor)->enter_func = enter_func;
    ((Visitor*)cvisitor)->leave_func = leave_func;

    return cvisitor;
}

void delete_codegen_visitor(CodegenVisitor* cvisitor) {    
    Visitor* visitor = (Visitor*)cvisitor;
    MEM_free(visitor->enter_expr_list);
    MEM_free(visitor->leave_expr_list);
    MEM_free(visitor->notify_expr_list);
    MEM_free(visitor->enter_stmt_list);
    MEM_free(visitor->leave_stmt_list);
    if (cvisitor->code) MEM_free(cvisitor->code);
    delete_jumptable(cvisitor->jtable);
    MEM_free(cvisitor);
}