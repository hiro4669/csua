
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


static void disasm(uint8_t *code, size_t size) {
    uint8_t *p = code;
    printf("%02x \n", *code);
    for (int i = 0; i < size; ++i, p++) {
        if (i % 16 == 0) printf("\n");
        printf("%02x ", *p);
    }
    printf("\n");

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