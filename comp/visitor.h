
#ifndef _VISITOR_H_
#define _VISITOR_H_

#include "csua.h"

typedef void (*visit_expr)(Expression* expr);
typedef void (*visit_stmt)(Statement*  stmt);


struct Visitor_tag {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
};

void print_depth();
Visitor* create_treeview_visitor();
void delete_visitor(Visitor* visitor);
void traverse_expr(Expression* expr, Visitor* visitor);
void traverse_stmt(Statement*  stmt, Visitor* visitor);

#endif