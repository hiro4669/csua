#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csua.h"
#include "visitor.h"
#include "../svm/svm.h"


static CS_Boolean is_jump(uint8_t op) {    
    switch (op) {
        case SVM_JUMP:
        case SVM_JUMP_IF_FALSE:
        case SVM_JUMP_IF_TRUE: {            
            return CS_TRUE;            
        }
        default: {
            return CS_FALSE;
        }
    }
    return CS_FALSE;
}

static void write_char(char c, FILE* fp) {
    fwrite(&c, 1, 1, fp);
}

static void write_reverse(const void* pv, size_t size, FILE* fp) {
    char* p = (char*)pv;
    for (int i = size - 1; i >= 0; --i) {
        write_char(p[i], fp);
    }
}

static void write_int(const uint32_t v, FILE* fp) {
    write_reverse(&v, sizeof(uint32_t), fp);
}

static void write_double(const double dv, FILE *fp) {
    write_reverse(&dv, sizeof(double), fp);    
}

static void write_bytes(uint8_t *p, int len, FILE* fp) {
    fwrite(p, 1, len, fp);
}

static uint16_t fetch2(uint8_t *code) {
    uint8_t first = *code; code++;
    uint8_t second = *code;
    return (uint16_t)(first << 8 | second);
}

void serialize(CS_Executable* exec) {
    fprintf(stderr, "--------------\n");
    fprintf(stderr, "serialize\n");
    FILE *fp;
    char* fname = "a.csb";
    if ((fp = fopen(fname, "wb")) == NULL) {
        fprintf(stderr, "Output file Error\n");
        exit(1);
    }
    /* Create Signature */
    write_char('C', fp);
    write_char('A', fp);
    write_char('F', fp);
    write_char('E', fp);
    write_char('S', fp);
    write_char('U', fp);
    write_char('A', fp);

    fprintf(stderr, "constant pool count = %d\n", exec->constant_pool_count);

    /* write constant*/
    write_int(exec->constant_pool_count, fp);
    for (int i = 0; i < exec->constant_pool_count; ++i) {
        switch (exec->constant_pool[i].type) {
            case CS_CONSTANT_INT: {
                write_char(SVM_INT, fp);
                write_int(exec->constant_pool[i].u.c_int, fp);
                break;
            }
            case CS_CONSTANT_DOUBLE: {
                write_char(SVM_DOUBLE, fp);
                write_double(exec->constant_pool[i].u.c_double, fp);
                break;
                }
            default: {
                fprintf(stderr, "undefined constant type\n");
                exit(1);
            }
        }
    }

    /* write function count */
    fprintf(stderr, "function count = %d\n", exec->function_count);

    /* write function arguments type */
    write_int(exec->function_count, fp);
    for (int i = 0; i < exec->function_count; ++i) {

        uint32_t total_val_count = exec->function[i].parameter_count + exec->function[i].local_variable_count;
        fprintf(stderr, "fname = %s\n", exec->function[i].name);
        fprintf(stderr, "parameter_count = %d\n", exec->function[i].parameter_count);
        fprintf(stderr, "local_val_count = %d\n", exec->function[i].local_variable_count);
        fprintf(stderr, "val_count = %d\n", total_val_count);
        write_int(total_val_count, fp);        
        for (int j = 0; j < exec->function[i].parameter_count; j++) {
            write_int(exec->function[i].parameter[j].type->basic_type, fp);
        }
        for (int j = 0; j < exec->function[i].local_variable_count; j++) {
            write_int(exec->function[i].local_variable[j].type->basic_type, fp);
        }
    }
    

    /* write global variables */
    write_int(exec->global_variable_count, fp);
    for (int i = 0; i < exec->global_variable_count; ++i) {
        write_int(exec->global_variable[i].type->basic_type, fp);
    }

    /* count all code size */
    uint32_t total_code_size = 0;
    for (int i = 0; i < exec->function_count; ++i) 
        total_code_size += exec->function[i].code_size;
    total_code_size += exec->code_size;

    fprintf(stderr, "total_code_size = %d\n", total_code_size);
    write_int(total_code_size, fp);

    /* create link table */
    LinkTable* ltable = create_linktable();
    uint32_t idx = 0;
    for (int i = 0; i < exec->function_count; ++i) {
        idx += exec->function[i].code_size;
        add_offset(ltable, idx);
        //fprintf(stderr, "table idx = %02x\n", idx);
    }

    idx = 0;
    /* write byte code for function */    
    for (int i = 0; i < exec->function_count; ++i) {
        uint32_t code_size = exec->function[i].code_size;
        for (int j = 0; j < code_size; ++j) {
            SVM_Opcode op = exec->function[i].code[j];
            OpcodeInfo oInfo = svm_opcode_info[op];
            write_char(op, fp);
            idx += 1;
            for (int k = 0; k < strlen(oInfo.parameter); ++k) {
                switch (oInfo.parameter[k]) {
                    case 'i': {
                        if (is_jump(op)) {
                            fprintf(stderr, "jump!!\n");
                        }
                        write_char(exec->function[i].code[++j], fp);
                        write_char(exec->function[i].code[++j], fp);
                        idx += 2;
                        break;
                    }
                    default: {
                        fprintf(stderr, "what?");
                        exit(1);
                    }
                }
            }            
        }
    }

    fprintf(stderr, "top code size = %d\n", exec->code_size);
    /* write toplevel byte code */
    for (int i = 0; i < exec->code_size; ++i) {
        SVM_Opcode op = exec->code[i];
        OpcodeInfo oInfo = svm_opcode_info[op];
        write_char(op, fp);
        idx += 1;
        for (int k = 0; k < strlen(oInfo.parameter); ++k) {
            switch (oInfo.parameter[k]) {
                case 'i': {
                    if (is_jump(op)) {
                        //fprintf(stderr, "jump main function\n");
                        uint16_t offset = get_offset(ltable, idx-1); // operator addr is idx-1
                        fprintf(stderr, "offset = %02x\n", offset);
                        if (offset != 0) {
                            uint16_t jaddr = fetch2(&exec->code[i+1]);
                            fprintf(stderr, "jaddr = %02x\n", jaddr);
                            //offset += jaddr;
                            jaddr += offset;
                            //fprintf(stderr, "offset = %02x\n", offset);
                            exec->code[i+1] = (jaddr >> 8) & 0xff;
                            exec->code[i+2] = (jaddr >> 0) & 0xff;                            
                        }
                    }
                    write_char(exec->code[++i], fp);
                    write_char(exec->code[++i], fp);
                    idx += 2;
                    break;
                }
                default: {
                    fprintf(stderr, "what top?\n");
                    exit(1);
                }
            }
        }
    }

    fprintf(stderr, "idx = %02x\n", idx);

    /* write byte code for function */
    /*
    for (int i = 0; i < exec->function_count; ++i) {
        uint32_t code_size = exec->function[i].code_size;
        
        for (int j = 0; j < code_size; j++) {
            SVM_Opcode op = exec->function[i].code[j];
            write_char(op, fp);
            //write_char(exec->function[i].code[j], fp);
        }        
    }
    */
    //fprintf(stderr, "top code size = %d\n", exec->code_size);
    /* write toplevel byte code */
    /*
    for (int i = 0; i < exec->code_size; ++i) {
        SVM_Opcode op = exec->code[i];
        write_char(op, fp);
        //write_char(exec->code[i], fp);
    }
    */

    delete_linktable(ltable);








}
