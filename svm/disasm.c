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
        fprintf(stderr, "name=%s, type=%d\n", variables[i].name, variables[i].type->basic_type);
    }
}

void disasm(uint8_t* code, uint32_t len) {
    fprintf(stderr, "-- Disassemblee --\n");
    fprintf(stderr, "-- code len(%d) --\n", len);
    for (uint32_t i = 0; i < len; ++i) {        
        switch(code[i]) {
            case SVM_PUSH_INT: {
                uint16_t idx = fetch2(&code[i+1]);                
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_PUSH_INT].opname, idx);
                i += 2;                
                break;
            }
            case SVM_PUSH_DOUBLE: {
                uint16_t idx = fetch2(&code[i+1]);
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_PUSH_DOUBLE].opname, idx);
                i += 2;
                break;
            }   
            case SVM_PUSH_STACK_INT: {
                uint16_t idx = fetch2(&code[i+1]);                
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_PUSH_STACK_INT].opname, idx);
                i += 2;   
                break;
            }
            case SVM_PUSH_STATIC_INT: {
                uint16_t idx = fetch2(&code[i+1]);                
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_PUSH_STATIC_INT].opname, idx);
                i += 2;                
                break;
            }
            case SVM_POP_STATIC_INT: {
                uint16_t idx = fetch2(&code[i+1]);
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_POP_STATIC_INT].opname, idx);
                i += 2;                
                break;
            }
            case SVM_ADD_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_ADD_INT].opname);
                break;
            }
            case SVM_ADD_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_ADD_DOUBLE].opname);
                break;
            }
            case SVM_SUB_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_SUB_INT].opname);
                break;
            }
            case SVM_SUB_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_SUB_DOUBLE].opname);
                break;
            }
            case SVM_MUL_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MUL_INT].opname);
                break;
            }
            case SVM_MUL_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MUL_DOUBLE].opname);
                break;
            }
            case SVM_DIV_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_DIV_INT].opname);
                break;
            }
            case SVM_DIV_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_DIV_DOUBLE].opname);
                break;
            }
            case SVM_MOD_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MOD_INT].opname);
                break;
            }
            case SVM_MOD_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MOD_DOUBLE].opname);
                break;
            }
            case SVM_POP_STACK_INT: {
                uint16_t idx = fetch2(&code[i+1]);
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_POP_STACK_INT].opname, idx);
                i += 2;
                break;
            }
            case SVM_INCREMENT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_INCREMENT].opname);
                break;
            }
            case SVM_DECREMENT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_DECREMENT].opname);
                break;
            }
            case SVM_CAST_INT_TO_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_CAST_INT_TO_DOUBLE].opname);
                break;
            }
            case SVM_CAST_DOUBLE_TO_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_CAST_DOUBLE_TO_INT].opname);
                break;
            }
            case SVM_MINUS_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MINUS_INT].opname);
                break;
            }
            case SVM_MINUS_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_MINUS_DOUBLE].opname);
                break;
            }
            case SVM_EQ_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_EQ_INT].opname);
                break;
            }
            case SVM_EQ_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_EQ_DOUBLE].opname);
                break;
            }
            case SVM_NE_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_NE_INT].opname);
                break;
            }
            case SVM_NE_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_NE_DOUBLE].opname);
                break;
            }
            case SVM_GT_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_GT_INT].opname);
                break;
            }
            case SVM_GT_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_GT_DOUBLE].opname);
                break;
            }
            case SVM_GE_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_GE_INT].opname);
                break;
            }
            case SVM_GE_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_GE_DOUBLE].opname);
                break;
            }
            case SVM_LT_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LT_INT].opname);
                break;
            }
            case SVM_LT_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LT_DOUBLE].opname);
                break;
            }
            case SVM_LE_INT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LE_INT].opname);
                break;
            }
            case SVM_LE_DOUBLE: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LE_DOUBLE].opname);
                break;
            }
            case SVM_LOGICAL_AND: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LOGICAL_AND].opname);
                break;
            }
            case SVM_LOGICAL_OR: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LOGICAL_OR].opname);
                break;
            }
            case SVM_LOGICAL_NOT: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_LOGICAL_NOT].opname);
                break;
            }
            case SVM_POP: {
                fprintf(stderr, "%02x:%s\n", i, svm_opcode_info[SVM_POP].opname);
                break;
            }
            case SVM_JUMP: {
                uint16_t idx = fetch2(&code[i+1]);
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_JUMP].opname, idx);
                i += 2;
                break;
            }
            case SVM_JUMP_IF_FALSE: {
                uint16_t idx = fetch2(&code[i+1]);
                fprintf(stderr, "%02x:%s %04x\n", i, svm_opcode_info[SVM_JUMP_IF_FALSE].opname, idx);
                i += 2;
                break;
            }            
            
            default: {
                fprintf(stderr, "unknown operator in disasm %02x\n", code[i]);
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