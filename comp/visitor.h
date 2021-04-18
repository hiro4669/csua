
#ifndef _VISITOR_H_
#define _VISITOR_H_

#include "csua.h"

/*
typedef void (*visit_expr)(Expression* expr);
typedef void (*visit_stmt)(Statement*  stmt);
typedef void (*visit_func)(FunctionDefinition* function);
*/

typedef void (*visit_expr)(Expression* expr, Visitor* visitor);
typedef void (*visit_stmt)(Statement*  stmt, Visitor* visitor);
typedef void (*visit_func)(FunctionDefinition* function, Visitor* visitor);

typedef void (*callback_stmt)(Statement* stmt, Visitor* visitor, StatementType s_type);



typedef struct MeanCheckLog_tag {
    char                    *log_str;
    struct MeanCheckLog_tag *next;
} MeanCheckLog;


typedef struct {
    uint16_t address;
} Table;

typedef struct {
    uint16_t idx;
    Table* tables;
} JumpTable;

struct Visitor_tag {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_expr* notify_expr_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;


    callback_stmt after_cond_func;
    callback_stmt end_block_func;

    visit_func enter_func;
    visit_func leave_func;
};

struct MeanVisitor_tag {
    Visitor       visitor;
    CS_Compiler  *compiler;
    MeanCheckLog *mean_log;
    Block        *block;
};

typedef enum {
    VISIT_NORMAL,
    VISIT_NORMAL_ASSIGN,
} VisitState;

struct CodegenVisitor_tag {
    Visitor        visitor;
    CS_Compiler   *compiler;
    CS_Executable *exec;
    JumpTable     *jtable;

    VisitState    v_state;
    uint16_t      assign_depth;
    uint32_t      CODE_ALLOC_SIZE;
    uint32_t      current_code_size;
    uint32_t      pos;
    uint8_t       *code;
};



Visitor* create_treeview_visitor();
MeanVisitor* create_mean_visitor();
CodegenVisitor* create_codegen_visitor(CS_Compiler* compiler, CS_Executable* exec);

void delete_visitor(Visitor* visitor);
void delete_mean_visitor(MeanVisitor* visitor);
void delete_codegen_visitor(CodegenVisitor* cvisitor);


void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement*  stmt, Visitor* visitor);
void traverse_func(FunctionDefinition* func, Visitor* visitor);


/* jumptablee.c */
JumpTable* create_jumptable();
void delete_jumptable(JumpTable* jtable);
uint16_t get_index(JumpTable* jtable);
void set_address(JumpTable* jtable, uint16_t idx, uint16_t addr);
uint16_t get_address(JumpTable* jtable, uint16_t idx);

#endif