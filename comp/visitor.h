
#ifndef _VISITOR_H_
#define _VISITOR_H_

#include "csua.h"

//typedef void (*visit_expr)(Expression* expr);
//typedef void (*visit_stmt)(Statement*  stmt);

typedef void (*visit_expr)(Expression* expr, Visitor* visitor);
typedef void (*visit_stmt)(Statement*  stmt, Visitor* visitor);


typedef struct MeanCheckLog_tag {
    char                    *log_str;
    struct MeanCheckLog_tag *next;
} MeanCheckLogger;

struct Visitor_tag {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
};

struct MeanVisitor_tag {
    Visitor visitor;
    CS_Compiler *compiler;
    int i;
    int j;
    MeanCheckLogger *check_log;
};

/* visitor.c */
void print_depth();
Visitor* create_treeview_visitor();
void delete_visitor(Visitor* visitor);
void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement*  stmt, Visitor* visitor);

/* mean_visitor */
MeanVisitor* create_mean_visitor();
void show_mean_error(MeanVisitor* visitor);
char* get_type_name(CS_BasicType type);

#endif