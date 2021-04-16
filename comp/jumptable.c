#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "visitor.h"
#include "../memory/MEM.h"

JumpTable* create_jumptable() {
    JumpTable* jtable = (JumpTable*)MEM_malloc(sizeof(JumpTable));
    jtable->idx = 0;
    jtable->tables = NULL;
    return jtable;
}

uint16_t get_index(JumpTable* jtable) {
    jtable->tables = MEM_realloc(jtable->tables, sizeof(Table) * (jtable->idx+1));
    jtable->tables[jtable->idx].address = 0;
    return jtable->idx++;
}

void set_address(JumpTable* jtable, uint16_t idx, uint16_t addr) {
    jtable->tables[idx].address = addr;
}

uint16_t get_address(JumpTable* jtable, uint16_t idx) {
    return jtable->tables[idx].address;
}

void delete_jumptable(JumpTable* jtable) {
    if (!jtable) return;
    if (jtable->tables) MEM_free(jtable->tables);
    MEM_free(jtable);
}