#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"
#include "../svm/svm.h"


static void copy_declaration(CS_Compiler* compiler, CS_Executable* exec) {
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size);

    CS_Variable* variables = (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;    
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {        
        variables[i].name = (char*)MEM_strdup(decl_list->decl->name);
        CS_TypeSpecifier* type = MEM_malloc(sizeof(CS_TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }
    exec->global_variable = variables;
    exec->global_variable_count = size;
}

static CS_Executable* create_executable(CS_Compiler* compiler) {
    CS_Executable* exec = (CS_Executable*)MEM_malloc(sizeof(CS_Executable));
    memset(exec, 0, sizeof(CS_Executable));
    exec->constant_pool_count = 0;
    exec->constant_pool = NULL;
    exec->global_variable_count = 0;
    exec->global_variable = NULL;
    exec->code_size = 0;
    exec->code = NULL;
    exec->function_count = 0;
    exec->function = NULL;

    copy_declaration(compiler, exec);

    return exec;    
}

static void delete_function(CS_Function* func) {
    MEM_free(func->type);
    MEM_free(func->name);
    for (int i = 0; i < func->parameter_count; ++i) {
        MEM_free(func->parameter[i].name);
        MEM_free(func->parameter[i].type);
    }

    if (func->parameter) MEM_free(func->parameter);
        
    for (int i = 0; i < func->local_variable_count; ++i) {
        MEM_free(func->local_variable[i].name);
        MEM_free(func->local_variable[i].type);
    }

    if (func->local_variable) MEM_free(func->local_variable);
    if (func->code) MEM_free(func->code);
}

static void delete_functions(CS_Executable* exec) {
    for (int i = 0; i < exec->function_count; ++i) {        
        delete_function(&exec->function[i]);        
    }
    if (exec->function_count > 0) {
        MEM_free(exec->function);
    }    
}


static void delete_executable(CS_Executable* exec) {
    delete_functions(exec);
    for (int i = 0; i < exec->global_variable_count; ++i) {
        MEM_free(exec->global_variable[i].name);
        MEM_free(exec->global_variable[i].type);
    }
    if (exec->global_variable) MEM_free(exec->global_variable);
    if (exec->constant_pool) MEM_free(exec->constant_pool);
    if (exec->code) MEM_free(exec->code);
    MEM_free(exec);
}

static CS_TypeSpecifier* copy_type(TypeSpecifier *type) {
    CS_TypeSpecifier *dtype = MEM_malloc(sizeof(CS_TypeSpecifier));
    dtype->basic_type = type->basic_type;
    return dtype;
}

/* Create CS_Function from FunctionDefinition */
static void copy_function(FunctionDefinition* src_fd, CS_Function* dest_fd) {
    dest_fd->type = copy_type(src_fd->type);
    dest_fd->name = MEM_strdup(src_fd->name);
    dest_fd->index = src_fd->index;
    dest_fd->parameter_count = 0;
    dest_fd->parameter = NULL;
    dest_fd->local_variable_count = 0;
    dest_fd->local_variable = NULL;
    dest_fd->code_size = 0;
    dest_fd->code = NULL;
    

    int i;
    int param_count = 0;
    ParameterList* param;
    for (param = src_fd->parameter; param; param = param->next) param_count++;

    dest_fd->parameter_count = param_count;

    /* parameteres are arguments */
    if (dest_fd->parameter_count > 0) {
        dest_fd->parameter = MEM_malloc(sizeof(CS_LocalVariable) * param_count);
        for (i = 0, param = src_fd->parameter; param; param = param->next, ++i) {
            dest_fd->parameter[i].name = MEM_strdup(param->name);
            dest_fd->parameter[i].type = copy_type(param->type);
        }
    }

    if (src_fd->block) {
        /* local_variables are variables defined in a function */
        int local_val_count = src_fd->local_variable_count - param_count;
        dest_fd->local_variable = MEM_malloc(sizeof(CS_LocalVariable) * local_val_count);
        dest_fd->local_variable_count = local_val_count;
        for (i = 0; i < local_val_count; ++i) {
            dest_fd->local_variable[i].name = MEM_strdup(src_fd->local_variable[i+param_count]->name);
            dest_fd->local_variable[i].type = copy_type(src_fd->local_variable[i+param_count]->type);
        }
    }
}

static void reset_gencode(CodegenVisitor* cvisitor) {
    cvisitor->code = NULL;
    cvisitor->pos = 0;
    cvisitor->current_code_size = 0;
}

static void add_function(CS_Compiler* compiler, CodegenVisitor* cvisitor) {
    //fprintf(stderr, "add function in suac.c\n");
    cvisitor->exec->function = MEM_malloc(sizeof(CS_Function) * compiler->function_count);
    cvisitor->exec->function_count = compiler->function_count;

    int i;
    FunctionDefinition* func;
    for (i = 0, func = compiler->function_list; func; func = func->next, ++i) {
        copy_function(func, &cvisitor->exec->function[i]);
        if (func->block) {
            StatementList* stmt_list = func->block->statement_list;
            while (stmt_list) {
                traverse_stmt(stmt_list->stmt, (Visitor*)cvisitor);
                stmt_list = stmt_list->next;
            }
            backpatch(cvisitor);
            cvisitor->exec->function[i].is_implemented = CS_TRUE;

            cvisitor->exec->function[i].code = cvisitor->code;
            cvisitor->exec->function[i].code_size = cvisitor->pos;

            reset_gencode(cvisitor);
        } else {
            cvisitor->exec->function[i].is_implemented = CS_FALSE;
        }
    }
    /*
    for (int j = 0; j < i; ++j) {
        fprintf(stderr, "name = %s\n", cvisitor->exec->function[j].name);
        fprintf(stderr, "parameter_cnt = %d\n", cvisitor->exec->function[j].parameter_count);
        fprintf(stderr, "local_variable_cnt = %d\n", cvisitor->exec->function[j].local_variable_count);
    }
    */


}


int main(int argc, char* argv[]) {

    int verbose = 0;
    int deser_flg = 0;

    FILE *fin;
    if (argc >= 2) {

        for (int i = 1; i < argc-1; ++i) {
            //fprintf(stderr, "op = %s\n", argv[i]);
            if (strcmp("-v", argv[i]) == 0) {
                verbose = 1;
            } else if (strcmp("-d", argv[i]) == 0) {
                deser_flg = 1;
            }
        }
    } else {
        fprintf(stderr, "Usage ./suac filename\n");
        exit(1);
    }


    if (deser_flg) {
        deserialize(argv[argc - 1]);
        exit(1);
    }

    fin = fopen(argv[argc - 1], "r");

    CS_Compiler* compiler = CS_create_compiler();
    CS_Boolean result = CS_compile(compiler, fin); // until mean check

    if (result) {
        /* begin code generate */
        CS_Executable* exec = create_executable(compiler);
        CodegenVisitor* cvisitor = create_codegen_visitor(compiler, exec);

        add_function(compiler, cvisitor);


        StatementList* stmt_list = compiler->stmt_list;
        while (stmt_list) {
            traverse_stmt(stmt_list->stmt, (Visitor*)cvisitor);
            stmt_list = stmt_list->next;
        }
        backpatch(cvisitor);

        exec->code = cvisitor->code;
        exec->code_size = cvisitor->pos;
        reset_gencode(cvisitor);

        /* Disassemble */
        if (verbose) {
            fprintf(stderr, "func count = %d\n", exec->function_count);
            for (int i = 0; i < exec->function_count; ++i) {
                fprintf(stderr, "--- function: %s ---\n", exec->function[i].name);
                fprintf(stderr, "\n--- parameters ---\n");
                show_local_variables(exec->function[i].parameter, exec->function[i].parameter_count);
                
                fprintf(stderr, "\n--- local variables ---\n");
                show_local_variables(exec->function[i].local_variable, exec->function[i].local_variable_count);
                fprintf(stderr, "\n");
                disasm(exec->function[i].code, exec->function[i].code_size);
            }
            fprintf(stderr, "\n--- main ---\n");
            show_variables(exec->global_variable, exec->global_variable_count);
            disasm(exec->code, exec->code_size);
        }

        /* Serialize Code */
        serialize(exec);
    
        delete_codegen_visitor(cvisitor);        
        delete_executable(exec);        
    } else {
        fprintf(stderr, "Error\n");
    }
        
    fclose(fin);
    CS_delete_compiler(compiler);
    MEM_dump_memory();    
    
    return 1;
}