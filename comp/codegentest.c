#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"
#include "../svm/svm.h"

static void copy_declaration(CS_Compiler* compiler, CS_Executable* exec) {
    fprintf(stderr, "copy decls\n");
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size);
    CS_Variable* variables = (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;    
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {
        fprintf(stderr, "yahoo\n");
        variables[i].name = (char*)MEM_strdup(decl_list->decl->name);
        CS_TypeSpecifier* type = MEM_malloc(sizeof(CS_TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }

    exec->global_variable = variables;
    exec->global_variable_count = size;
    // ToDo copy function's variables
}


static CS_Executable* code_generate(CS_Compiler* compiler) {
    CS_Executable* exec = (CS_Executable*)MEM_malloc(sizeof(CS_Executable));
    memset(exec, 0, sizeof(CS_Executable));
    exec->constant_pool_count = 0;
    exec->constant_pool = NULL;
    exec->global_variable_count = 0;
    exec->global_variable = NULL;
    exec->code_size = 0;
    exec->code = NULL;

    copy_declaration(compiler, exec);

    return exec;
}

static void delete_executable(CS_Executable* exec) {
    fprintf(stderr, "count = %d\n", exec->global_variable_count);
    for (int i = 0; i < exec->global_variable_count; ++i) {
        MEM_free(exec->global_variable[i].name);
        MEM_free(exec->global_variable[i].type);
    }
    MEM_free(exec->global_variable);
    MEM_free(exec->constant_pool);

    MEM_free(exec);
}

int main(void) {

    FILE *fin = fopen("tests/prog2.cs", "r");
    CS_Compiler* compiler = CS_create_compiler();
    CS_Boolean result = CS_compile(compiler, fin);

    if (result) {
        printf("execute code generate\n");
        CS_Executable* exec = code_generate(compiler);

        CodegenVisitor* cvisitor = create_codegen_visitor(compiler, exec);

        StatementList* stmt_list = compiler->stmt_list;
        while (stmt_list) {
            traverse_stmt(stmt_list->stmt, (Visitor*)cvisitor);
            stmt_list = stmt_list->next;
        }

        // for test
        fprintf(stderr, "code len = %d\n", cvisitor->pos);
        for (int i = 0; i < cvisitor->pos; ++i) {
            fprintf(stderr, "%02x ", cvisitor->code[i]);
        }
        fprintf(stderr, "\n");

        // for test
        show_variables(exec->global_variable, exec->global_variable_count);
        disasm(cvisitor->code, cvisitor->pos);
        

     
        delete_codegen_visitor(cvisitor);
        delete_executable(exec);
    }

    fclose(fin);
    CS_delete_compiler(compiler);
    MEM_dump_memory();

    return 0;
}
