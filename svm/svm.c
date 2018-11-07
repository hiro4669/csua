
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "svm.h"


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


static void disasm(uint8_t *code, size_t size) {
    uint8_t *p = code;
    DInfo dinfo = {0};
    int param_len = 0;
    
    for (int i = 0; i < size; ++i, p++) {
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


static void parse(uint8_t* buf) {
    uint8_t* pos = buf;
    parse_header(&pos);
    int constant_pool_count = read_int(&pos);
    printf("constant_pool_count = %d\n", constant_pool_count);
    uint8_t type;
    for (int i = 0; i < constant_pool_count; ++i) {
        switch(type = read_byte(&pos)) {
            case SVM_INT: {
                int v = read_int(&pos);
                break;
            }
            case SVM_DOUBLE: {
                double dv = read_double(&pos);
                break;
            }
            default: {
                fprintf(stderr, "undefined constant type\n in parse");
                exit(1);
            }
        }
    }
    int global_variable_count = read_int(&pos);
    printf("global_variable_count = %d\n", global_variable_count);
    for (int i = 0; i < global_variable_count; ++i) {
        uint8_t type = read_byte(&pos);
        switch (type) {
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
    
    
    int code_size = read_int(&pos);
    printf("code_size = %d\n", code_size);
    disasm(pos, code_size);
}


int main(int argc, char *argv[]) {

    // for test
    bool disasm = false;
    
    if (argc < 2) {
        fprintf(stderr, "Usage ./svm [optino] file\n");
        exit(1);
    }
    
    if (argc == 3) {
        if (!strcmp("-d", argv[1])) {
          printf("disasm\n");  
          disasm = true;                  
        } else {
            fprintf(stderr, "No such option)\n");
        }
    }
    
    if (disasm) {
        struct stat st;
        stat(argv[2], &st);
        printf("size = %d\n", (int)st.st_size);
        uint8_t* buf = (uint8_t*)malloc(st.st_size);
        int fp = open(argv[2], O_RDONLY); 
        read(fp, buf, st.st_size);

        parse(buf);
        
        close(fp);
        free(buf);
        return 0;
    }
    
    
    return 0;
}