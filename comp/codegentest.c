#include <stdio.h>
#include <stdlib.h>
#include "csua.h"
#include "visitor.h"
#include "../memory/MEM.h"



static void copy_declaration(CS_Compiler* compiler, CS_Executable* exec) {
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size);
    printf("decl len = %d\n", size);
    CS_Variable* variables = (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {
        variables[i].name = MEM_strdup(decl_list->decl->name);
        TypeSpecifier* type = MEM_malloc(sizeof(TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }
    exec->global_variable = variables;
}

static void code_generate(CS_Compiler* compiler) {
    CS_Executable* exec = (CS_Executable*)MEM_malloc(sizeof(CS_Executable));
    copy_declaration(compiler, exec);
    /*
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size);
    printf("decl len = %d\n", size);
    CS_Variable* variables = (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {
        variables[i].name = MEM_strdup(decl_list->decl->name);
        TypeSpecifier* type = MEM_malloc(sizeof(TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }
    */
       
    // delete test
    /*
    for(int i = 0; i < size; ++i) {
        MEM_free(variables[i].name);
        MEM_free(variables[i].type);
    }
    MEM_free(variables);
     */
    
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
    
    code_generate(compiler);
    
    
    if (compile_result) {        
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