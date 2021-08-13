
#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"



CS_Compiler* CS_create_compiler() {
    MEM_Storage storage;
    CS_Compiler *compiler;    
    storage = MEM_open_storage(0);
    compiler = (CS_Compiler*)MEM_storage_malloc(storage, sizeof(CS_Compiler));
    compiler->storage = storage;
    compiler->expr_list = NULL;
    compiler->stmt_list = NULL;
    compiler->decl_list = NULL;
    compiler->current_block = NULL;
    compiler->function_count = 0;
    compiler->function_list = NULL;
    compiler->current_line = 1;
    
    cs_set_current_compiler(compiler);
    
    return compiler;
}

void CS_delete_compiler(CS_Compiler* compiler) {

    FunctionDefinition *fpos;
    for (fpos = compiler->function_list; fpos; fpos = fpos->next) {
        if (fpos->local_variable) MEM_free(fpos->local_variable);
    }
    MEM_Storage storage = compiler->storage;
    MEM_dispose(storage);
    //fprintf(stderr, "end of delete_compiler\n");
}

static void add_return_statement(FunctionDefinition* function) {
    //fprintf(stderr, "-- add return statement --\n");
    //fprintf(stderr, "function name = %s\n", function->name);

    if (function->block) {
        StatementList* stmt_list = function->block->statement_list;
        if (stmt_list) {
            for (; stmt_list->next; stmt_list = stmt_list->next)
                ;       
            if (stmt_list->stmt->type == RETURN_STATEMENT) {
                //fprintf(stderr, "already return exists\n");
                return; // do nothing
            }
            //fprintf(stderr, "add return to the end\n");
            Statement* rstmt = cs_create_return_statement(NULL);
            cs_chain_statement_list(stmt_list, rstmt);
        } else {
            //fprintf(stderr, "create and add\n");
            Statement* rstmt = cs_create_return_statement(NULL);
            function->block->statement_list = cs_create_statement_list(rstmt);
        }
    }
    return;
}


static CS_Boolean do_mean_check(CS_Compiler* compiler) {
    CS_Boolean result;
    MeanVisitor* mvisitor = create_mean_visitor();
    FunctionDefinition* function = compiler->function_list;
    while (function) {
        add_return_statement(function);
        traverse_func(function, (Visitor*)mvisitor);
        function = function->next;
    }

    StatementList* stmt_list = compiler->stmt_list;
    while (stmt_list) {
        traverse_stmt(stmt_list->stmt, (Visitor*)mvisitor);
        stmt_list = stmt_list->next;
    }

    if (mvisitor->mean_log) {
        result = CS_FALSE;
        MeanCheckLog* mlog;
        for (mlog = mvisitor->mean_log; mlog; mlog = mlog->next) {
            fprintf(stderr, "%s\n", mlog->log_str);
        }
    } else {        
        result = CS_TRUE;
    }
    
    delete_mean_visitor(mvisitor);

    DeclarationList* decls;
    int idx = 0;
    for (decls = compiler->decl_list; decls; decls = decls->next) {
        decls->decl->index = idx++;
        decls->decl->is_local = CS_FALSE;
    }


    return result;

}

CS_Boolean CS_compile(CS_Compiler* compiler, FILE *fin) {
    extern int yyparse(void);
    extern FILE *yyin;
    yyin = fin;
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }
    if (yyparse()) {
        fprintf(stderr, "Parse Error");
        exit(1);
    }

    return do_mean_check(compiler);

}

