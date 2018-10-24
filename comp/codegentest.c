#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"



static void copy_declaration(CS_Compiler* compiler, CS_Executable* exec) {
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size);
    CS_Variable* variables = (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {
        variables[i].name = MEM_strdup(decl_list->decl->name);
        TypeSpecifier* type = MEM_malloc(sizeof(TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }
    exec->global_variable = variables;
    exec->global_variable_count = size;
}

static CS_Executable* code_generate(CS_Compiler* compiler) {
    CS_Executable* exec = (CS_Executable*)MEM_malloc(sizeof(CS_Executable));
    memset(exec, 0x0, sizeof(CS_Executable));
    exec->code = NULL;
    exec->code_size = 0;
    exec->constant_pool = NULL;
    exec->constant_pool_count = 0;
    exec->global_variable = NULL;
    exec->global_variable_count = 0;
    
    
    copy_declaration(compiler, exec); // copy variables
    CodegenVisitor* cgen_visitor = create_codegen_visitor(compiler, exec);
    
    StatementList* stmt_list = compiler->stmt_list;
    while(stmt_list) {
        traverse_stmt(stmt_list->stmt, (Visitor*)cgen_visitor);
        stmt_list = stmt_list->next;
    }
    
    exec->code_size = cgen_visitor->pos;
    exec->code = (uint8_t*)MEM_malloc(exec->code_size);
    memcpy(exec->code, cgen_visitor->code, exec->code_size);
    
    if (cgen_visitor->code) {
        MEM_free(cgen_visitor->code);
    }
    delete_visitor((Visitor*)cgen_visitor);
    
    return exec;        
}

static void delete_executable(CS_Executable* exec) {
    for(int i = 0; i < exec->global_variable_count; ++i) {
        MEM_free(exec->global_variable[i].name);
        MEM_free(exec->global_variable[i].type);
    }
    MEM_free(exec->global_variable);
    
    if (exec->constant_pool != NULL) {
        MEM_free(exec->constant_pool);
    }
    
    if (exec->code != NULL) {
        MEM_free(exec->code);
    }
    
    
    MEM_free(exec);
}

static void exec_disasm(CS_Executable* exec) {
    fprintf(stderr, "< Disassemble Start >\n");
    fprintf(stderr, "-- global variables --\n");
    for (int i = 0; i < exec->global_variable_count; ++i) {
        fprintf(stderr, "%s:%s ", exec->global_variable[i].name, get_type_name(exec->global_variable[i].type->basic_type));
        if (i % 10 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    
    fprintf(stderr, "-- code --\n");
    for (int i = 0; i < exec->code_size; ++i) {
        if (i % 16 == 0) fprintf(stderr, "\n");
        fprintf(stderr, "%02x ", exec->code[i]);        
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "< Disassemble End >\n");
}




int main(int argc, char* argv[]) {
    
    printf("arg len = %d\n", argc);

    if (argc == 1) {
        printf("Usage ./meant dir/filename.cs\n");
        return 1;        
    }
    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL) {
        printf("Cannot fine file %s\n", argv[1]);
        return 1;
    }
    CS_Compiler* compiler = CS_create_compiler();
    CS_Boolean compile_result = CS_compile(compiler, fin);
    
    
    
    
    if (compile_result) {
        // Code Generate
        CS_Executable* exec = code_generate(compiler);
        exec_disasm(exec);
        delete_executable(exec);
        
        printf("--------------\n");
        Visitor* visitor = create_treeview_visitor();    
        StatementList* stmt_list = compiler->stmt_list;
        while(stmt_list) {
            traverse_stmt(stmt_list->stmt, visitor);
            stmt_list = stmt_list->next;
        }
        delete_visitor(visitor);
    }    
    
    fclose(fin);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}