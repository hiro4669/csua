#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../memory/MEM.h"
#include "visitor.h"

LinkTable* create_linktable() {
    LinkTable* ltable = (LinkTable*)MEM_malloc(sizeof(LinkTable));
    ltable->idx = 0;
    ltable->tables = NULL;
    return ltable;
}

void add_offset(LinkTable* ltable, uint16_t addr) {
    ltable->tables = MEM_realloc(ltable->tables, sizeof(Table) * (ltable->idx+1));
    ltable->tables[ltable->idx++].address = addr;
}

uint16_t get_func_addr(LinkTable* ltable, int idx) {    
    if (idx >= ltable->idx) {
        fprintf(stderr, "idx is out of range in get_func_addr\n");
        exit(1);
    }
    if (idx < 0) return 0;
    return ltable->tables[idx].address;
}

uint16_t get_offset(LinkTable* ltable, uint16_t addr) {
    //fprintf(stderr, "get_offset: %02x\n", addr);
    //fprintf(stderr, "1: %02x\n", ltable->tables[0].address);
    int target_idx = -1;    
    for (int i = 0; i < ltable->idx; ++i) {
        if (ltable->tables[i].address < addr) {
            target_idx = i;
        }
        /*
        if (ltable->tables[i].address >= addr) {
            return ltable->tables[i].address;
        }
        */
    }

    if (target_idx == -1) {
        return 0;
    } else {
        return ltable->tables[target_idx].address;
    }
    
    
    //return 0;
    //return ltable->tables[idx].address;
}

void delete_linktable(LinkTable* ltable) {
    if (!ltable) return;

    if (ltable->tables) MEM_free(ltable->tables);
    MEM_free(ltable);

}