#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "svm.h"
#include "../memory/MEM.h"


static int read_int(uint8_t **p) {
    uint8_t v1 = **p; (*p)++;
    uint8_t v2 = **p; (*p)++;
    uint8_t v3 = **p; (*p)++;
    uint8_t v4 = **p; (*p)++;        
    int v =(int)( v1 << 24 | v2 << 16 | v3 << 8 | v4);
    return v;
}

static double read_double(uint8_t **p) {
    uint8_t v1 = **p; (*p)++;
    uint8_t v2 = **p; (*p)++;
    uint8_t v3 = **p; (*p)++;
    uint8_t v4 = **p; (*p)++;
    uint8_t v5 = **p; (*p)++;
    uint8_t v6 = **p; (*p)++;
    uint8_t v7 = **p; (*p)++;
    uint8_t v8 = **p; (*p)++;
    
    long lv = (long)((long)v1 << 56 | (long)v2 << 48 | (long)v3 << 40 | (long)v4 << 32
			| (long)v5 << 24 | (long)v6 << 16 | (long)v7 << 8 | (long)v8);    
    double* pdv = (double*)&lv;
    return *pdv;

}

static uint8_t read_byte(uint8_t **p) {
    uint8_t v = **p; (*p)++;
    return v;
}

static void parse_header(uint8_t **p) {
    for(int i = 0; i < 8; ++i) {
        printf("%c ", (char)(**p));
        (*p)++;
    }
    printf("\n");        
}

typedef struct {
    char s_buf[128];
    char row_buf[32];    
    int s_index;
    int r_index;
    
} DInfo;

static void dump(DInfo *info) {
    fprintf(stderr, "%s", info->s_buf);
    fprintf(stderr, "   %s\n", info->row_buf);
    info->s_index = 0;
    info->r_index = 0;
}

static void add_opname(DInfo *info, const char* str) {
    int len = strlen(str);
    strncpy(&info->s_buf[info->s_index], str, len);
    info->s_index += len;
    for (int i = 0; i < (20-len); ++i, ++info->s_index) {
        info->s_buf[info->s_index] = ' ';
    }        
    info->s_buf[info->s_index] = 0;
}

static void add_padding(DInfo *info) {
    for(int i = 0; i < 4; ++i, ++info->s_index) info->s_buf[info->s_index] = ' ';    
}

static void add_uint16(DInfo *info, const uint16_t iv) {
    char buf[6] = {};
    sprintf(buf, "%04x", iv);
    strncpy(&info->s_buf[info->s_index], buf, 4);
}

static void add_rowcode(DInfo *info, uint8_t op) {
//    printf("add row %02x\n", op);
    sprintf(&info->row_buf[info->r_index], "%02x", op);
    info->r_index += 2;
    info->row_buf[info->r_index] = 0;
}


static void disasm(SVM_VirtualMachine* svm) {
//static void disasm(uint8_t *code, size_t size) {
    uint8_t *p = svm->code;
    DInfo dinfo = {0};
    int param_len = 0;
    
    for (int i = 0; i < svm->code_size; ++i, p++) {
        OpcodeInfo *oinfo = &svm_opcode_info[*p];
        add_rowcode(&dinfo, *p);
        switch(*p) {
            case SVM_PUSH_INT:
            case SVM_POP_STATIC_INT: 
            case SVM_PUSH_STACK_INT:
            case SVM_PUSH_FUNCTION:
            case SVM_POP:
            case SVM_ADD_INT:
            case SVM_INVOKE: {
//                printf("%s\n", oinfo->opname);
                add_opname(&dinfo, oinfo->opname);
                break;
            }
            default: {
                fprintf(stderr, "unknown opcode [%02x] in disasm\n", *p);
                exit(1);
            }
        }
        param_len = strlen(oinfo->parameter);
        for (int j = 0; j < param_len; ++j) {
            switch(oinfo->parameter[j]) {
                case 'i': {
                    uint8_t uv = *(++p);
                    uint16_t op = (uint16_t)( uv << 8 | *(++p));
                    add_uint16(&dinfo, op);
                    add_rowcode(&dinfo, uv);
                    add_rowcode(&dinfo, *p);                     
                    i += 2;
                    break;
                }
                default: {
                    fprintf(stderr, "unknown opcode [%c] in disasm\n", oinfo->parameter[j]);
                    exit(1);
                }
            }
        }
        if (param_len == 0) add_padding(&dinfo);
        
        dump(&dinfo);        
    }    
}


static void parse(uint8_t* buf, SVM_VirtualMachine* svm) {
    uint8_t* pos = buf;
    parse_header(&pos);
    svm->constant_pool_count = read_int(&pos);
    printf("constant_pool_count = %d\n", svm->constant_pool_count);
    svm->constant_pool = (SVM_Constant*)MEM_malloc(sizeof(SVM_Constant) * svm->constant_pool_count);
    
    uint8_t type;
    for (int i = 0; i < svm->constant_pool_count; ++i) {
        switch(type = read_byte(&pos)) {
            case SVM_INT: {
                int v = read_int(&pos);
                svm->constant_pool[i].type = SVM_INT;
                svm->constant_pool[i].u.c_int = v;
                break;
            }
            case SVM_DOUBLE: {
                double dv = read_double(&pos);
                svm->constant_pool[i].type = SVM_DOUBLE;
                svm->constant_pool[i].u.c_double = dv;                
                break;
            }
            default: {
                fprintf(stderr, "undefined constant type\n in parse");
                exit(1);
            }
        }
    }
    
    svm->global_variable_count = read_int(&pos);
    svm->global_variables = (SVM_Value*)MEM_malloc(sizeof(SVM_Value) * svm->global_variable_count);
    svm->global_variable_types = (uint8_t*)MEM_malloc(sizeof(uint8_t) * svm->global_variable_count);
    printf("global_variable_count = %d\n", svm->global_variable_count);
    for (int i = 0; i < svm->global_variable_count; ++i) {        
//        uint8_t type = read_byte(&pos);
        svm->global_variable_types[i] = read_byte(&pos);
        switch (svm->global_variable_types[i]) {
            case SVM_INT: {
                printf("INT\n");
                break;
            }
            case SVM_DOUBLE: {
                printf("DOUBLE\n");                
                break;
            }
            defulat: {
                break;
            }
        }
    }
    
    svm->code_size = read_int(&pos);
    printf("code_size = %d\n", svm->code_size);
    svm->code = (uint8_t*)MEM_malloc(svm->code_size);
    memcpy(svm->code, pos, svm->code_size);
    pos += svm->code_size;
    svm->stack_size = read_int(&pos);
    printf("stack_size = %d\n", svm->stack_size);    
    
//    svm->code = pos;
 //   disasm(pos, svm->code_size);
 
}


static SVM_VirtualMachine* svm_create() {
    SVM_VirtualMachine* svm = (SVM_VirtualMachine*)MEM_malloc(sizeof(SVM_VirtualMachine));
    svm->constant_pool = NULL;
    svm->global_variables = NULL;
    svm->global_variable_types = NULL;
    svm->code = NULL;
    svm->constant_pool_count = 0;
    svm->global_variable_count = 0;
    svm->code_size = 0;
    svm->function_count = 0;
    svm->functions = NULL;
    return svm;
}

static void svm_delete(SVM_VirtualMachine* svm) {
    if (!svm) return;
    if (svm->code) {
        MEM_free(svm->code);
    }
    if (svm->constant_pool) {
        MEM_free(svm->constant_pool);
    }
    if (svm->global_variables) {
        MEM_free(svm->global_variables);
    }
    if (svm->global_variable_types) {
        MEM_free(svm->global_variable_types);
    }
    if (svm->functions) {
        MEM_free(svm->functions);
    }
    
    MEM_free(svm);
}

void svm_add_native_function(SVM_VirtualMachine* svm, SVM_NativeFunction native_f, char* name, int arg_count) {
    svm->functions = (SVM_Function*)MEM_realloc(svm->functions, sizeof(SVM_Function) * (svm->function_count+1));
    svm->functions[svm->function_count].f_type = NATIVE_FUNCTION;
    svm->functions[svm->function_count].name = name;
    svm->functions[svm->function_count].arg_count = arg_count;
    svm->functions[svm->function_count].u.n_func = native_f;
    svm->function_count++;    
}



int main(int argc, char *argv[]) {

    // for test
    bool disasm_mode = false;
    
    if (argc < 2) {
        fprintf(stderr, "Usage ./svm [optino] file\n");
        exit(1);
    }
    
    if (argc == 3) {
        if (!strcmp("-d", argv[1])) {
          printf("disasm\n");  
          disasm_mode = true;                  
        } else {
            fprintf(stderr, "No such option)\n");
        }
    }
    
    SVM_VirtualMachine* svm = svm_create();
    struct stat st;
    stat(argv[2], &st);
    printf("size = %d\n", (int)st.st_size);
    uint8_t* buf = (uint8_t*)malloc(st.st_size);
    int fp = open(argv[2], O_RDONLY); 
    read(fp, buf, st.st_size);
    parse(buf, svm);        
    close(fp);
    free(buf);
    
    add_native_functions(svm);
    
    if (disasm_mode) {
        disasm(svm);
    }
    
    svm_delete(svm);
    MEM_dump_memory();
    
    
    return 0;
}