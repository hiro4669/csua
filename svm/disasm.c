#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#include "svm.h"

static uint16_t fetch2(uint8_t* code) {
    uint8_t upper = code[0];
    uint8_t lower = code[1];
    return (upper << 8 | lower) & 0xffff;
}

void show_variables(CS_Variable* variables, int size) {
    fprintf(stderr, "-- variables --\n");    
    for (int i = 0; i < size; ++i) {
        fprintf(stderr, "name=%s, type=%d\n", variables->name, variables->type->basic_type);
    }
}

void disasm(uint8_t* code, uint32_t len) {
    fprintf(stderr, "-- code --\n");
    for (uint32_t i = 0; i < len; ++i) {
        switch(code[i]) {
            case SVM_PUSH_INT: {
                uint16_t idx = fetch2(&code[i+1]);                
                fprintf(stderr, "%s %04x\n", svm_opcode_info[SVM_PUSH_INT].opname, idx);
                i += 2;                
                break;
            }
            case SVM_ADD_INT: {
                fprintf(stderr, "%s\n", svm_opcode_info[SVM_ADD_INT].opname);
                break;
            }
            case SVM_PUSH_STATIC_INT: {
                uint16_t idx = fetch2(&code[i+1]);                
                fprintf(stderr, "%s %04x\n", svm_opcode_info[SVM_PUSH_STATIC_INT].opname, idx);
                i += 2;                
                break;
            }

            default: {
            fprintf(stderr, "unknown operator %02x\n", code[i]);
                exit(1);
            }
            
        }
    }

}
/*
int main(void) {

    OpcodeInfo oInfo = svm_opcode_info[0];

    printf("name = %s\n", oInfo.opname);


    return 0;
}
*/