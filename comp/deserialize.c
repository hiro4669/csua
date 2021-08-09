#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../svm/svm.h"

static int read_int(const char* buf, int *idx) {
    uint8_t v1 = buf[(*idx)++];
    uint8_t v2 = buf[(*idx)++];
    uint8_t v3 = buf[(*idx)++];
    uint8_t v4 = buf[(*idx)++];
    int val = (int)(v1 << 24 | v3 << 16 | v2 << 8 | v4);
    return val;
}

static double read_double(const char* buf, int *idx) {
    uint8_t v1 = buf[(*idx)++];
    uint8_t v2 = buf[(*idx)++];
    uint8_t v3 = buf[(*idx)++];
    uint8_t v4 = buf[(*idx)++];
    uint8_t v5 = buf[(*idx)++];
    uint8_t v6 = buf[(*idx)++];
    uint8_t v7 = buf[(*idx)++];
    uint8_t v8 = buf[(*idx)++];

    long lv = (long)((long)v1 << 56 | (long)v2 << 48 | (long)v3 << 40 | (long)v4 << 32
			| (long)v5 << 24 | (long)v6 << 16 | (long)v7 << 8 | (long)v8);    
    double* pdv = (double*)&lv;
    return *pdv;
}

static uint8_t read_char(const char* buf, int *idx) {
    return buf[(*idx)++];
}


static void read_header(const char* buf, int *idx) {
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
    fprintf(stderr, "%c\n", buf[(*idx)++]);
}

void deserialize(char* fname) {

    struct stat s_buf;
    char* buf;
    int fd;
    int size;
    int idx = 0;
    
    fprintf(stderr, "deserialize = %s\n", fname);
    int status = stat(fname, &s_buf);
    fprintf(stderr, "size = %lld\n", s_buf.st_size);
    buf = (char*)malloc(s_buf.st_size);
    fd = open(fname, O_RDONLY);
    size = read(fd, buf, s_buf.st_size);
    fprintf(stderr, "size = %d\n", size);
    close(fd);

    read_header(buf, &idx);
    int const_size = read_int(buf, &idx);

    /* constant pool count */
    fprintf(stderr, "constant_pool_size = %x\n", const_size);

    /* constant pool type and value */
    for (int i = 0; i < const_size; ++i) {
        uint8_t type = read_char(buf, &idx);
        switch (type) {
            case SVM_INT: {
                int iv = read_int(buf, &idx);
                fprintf(stderr, "ival = %d\n", iv);
                break;
            }
            case SVM_DOUBLE: {                
                double dv = read_double(buf, &idx);
                fprintf(stderr, "dval = %f\n", dv);                
                break;
            }
            default: {
                break;
            }
        }
    }

    int function_count = read_int(buf, &idx);
    fprintf(stderr, "function count = %d\n", function_count);
    for (int i = 0; i < function_count; ++i) {
        int total_val_count = read_int(buf, &idx);
        fprintf(stderr, "total_val_count = %d\n", total_val_count);
        for (int j = 0; j < total_val_count; ++j) {
            int type = read_int(buf, &idx);
            switch (type) {
                case CS_BOOLEAN_TYPE: {
                    fprintf(stderr, "-- boolean\n");
                    break;
                }
                case CS_INT_TYPE: {
                    fprintf(stderr, "-- int\n");
                    break;
                }
                case CS_DOUBLE_TYPE: {
                    fprintf(stderr, "-- double\n");
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    int global_variable_count = read_int(buf, &idx);
    fprintf(stderr, "global_variable_count = %d\n", global_variable_count);
    for (int i = 0; i < global_variable_count; ++i) {
        int type = read_int(buf, &idx);
        switch (type) {
            case CS_BOOLEAN_TYPE: {
                fprintf(stderr, "-- boolean\n");
                break;
            }
            case CS_INT_TYPE: {
                fprintf(stderr, "-- int\n");
                break;
            }
            case CS_DOUBLE_TYPE: {
                fprintf(stderr, "-- double\n");
                break;
            }
            default: {
                break;
            }            
        }   
    }
    int total_code_size = read_int(buf, &idx);
    fprintf(stderr, "total_code_size = %d\n", total_code_size);
    fprintf(stderr, "idx = %d\n", idx);
    // for test
    int l_idx = idx;
    for (int i = 0; i < total_code_size; ++i) {
        if (i % 16 == 0) fprintf(stderr, "\n");
        fprintf(stderr, "%02x ", buf[l_idx++]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "idx = %d\n", idx);
    disasm((uint8_t*)&buf[idx], total_code_size);





    



    free(buf);
}