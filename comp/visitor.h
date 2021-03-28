
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


typedef struct MeanCheckLog_tag {
    char                    *log_str;
    struct MeanCheckLog_tag *next;
} MeanCheckLog;

struct Visitor_tag {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;

    visit_func enter_func;
    visit_func leave_func;
};

struct MeanVisitor_tag {
    Visitor       visitor;
    CS_Compiler  *compiler;
    MeanCheckLog *mean_log;
    Block        *block;
};

Visitor* create_treeview_visitor();
MeanVisitor* create_mean_visitor();
void delete_visitor(Visitor* visitor);
void delete_mean_visitor(MeanVisitor* visitor);
void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement*  stmt, Visitor* visitor);
void traverse_func(FunctionDefinition* func, Visitor* visitor);

#endif