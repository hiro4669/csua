#include <stdio.h>
#include <stdlib.h>

#include "csua.h"
#include "../svm/svm.h"

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

void serialize(CS_Executable* exec) {
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
        fprintf(stderr, "val_count = %d\n", total_val_count);
        write_int(total_val_count, fp);
        for (int j = 0; j < exec->function[i].parameter_count; j++) {
            write_int(exec->function[i].parameter[j].type->basic_type, fp);
        }
        for (int j = 0; j < exec->function[i].local_variable_count; j++) {
            write_int(exec->function[i].local_variable[i].type->basic_type, fp);
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

    
    /* write byte code for function */    
    uint32_t pc_idx = 0;
    for (int i = 0; i < exec->function_count; ++i) {
        uint32_t code_size = exec->function[i].code_size;
        for (int j = 0; j < code_size; j++) {
            write_char(exec->function[i].code[j], fp);
        }
        pc_idx += code_size;
    }

    fprintf(stderr, "pc = %d\n", pc_idx);
    fprintf(stderr, "top code size = %d\n", exec->code_size);
    /* write toplevel byte code */
    for (int i = 0; i < exec->code_size; ++i) {
        write_char(exec->code[i], fp);
    }








}
