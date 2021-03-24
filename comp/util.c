
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csua.h"

static CS_Compiler *current_compiler = NULL;

void cs_set_current_compiler(CS_Compiler *compiler) {
    current_compiler = compiler;
}

CS_Compiler* cs_get_current_compiler() {
    return current_compiler;
}


FunctionDefinition* cs_search_function(char *name) {
    FunctionDefinition *pos;
    CS_Compiler *compiler = cs_get_current_compiler();

    for (pos = compiler->function_list; pos; pos=pos->next) {
        if (!strcmp(pos->name, name)) {
                break;
        }
    }
    return pos;
}

Declaration* cs_search_declaration(char *name, Block *block) {
    Block *bpos;
    DeclarationList *dpos;
    CS_Compiler *compiler;

    for (bpos = block; bpos; bpos = bpos->outer_block) {
        for (dpos = bpos->declaration_list; dpos; dpos = dpos->next) {
            if (!strcmp(dpos->decl->name, name)) {
                return dpos->decl;
            }
        }
    }

    compiler = cs_get_current_compiler();

    for (dpos = compiler->decl_list; dpos; dpos = dpos->next) {
        if (!strcmp(dpos->decl->name, name)) {
            return dpos->decl;
        }
    }    
    return NULL;
}


DeclarationList* cs_chain_declaration(DeclarationList *decl_list, Declaration* decl) {
    DeclarationList *pos;
    DeclarationList* list = cs_create_declaration_list(decl);
    if (!list) {
        fprintf(stderr, "cannot create declaration list\n");
        exit(1);
    }
    if (decl_list == NULL) return list;

    for (pos = decl_list; pos->next; pos = pos->next);
    pos->next = list;
    list->next = NULL;

    return decl_list;

}

