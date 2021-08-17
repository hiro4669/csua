#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../memory/MEM.h"
#include "svm.h"

static SVM_VirtualMachine* svm_create() {
    SVM_VirtualMachine* svm = (SVM_VirtualMachine*)MEM_malloc(sizeof(SVM_VirtualMachine));
    svm->constant_pool_count = 0;
    svm->constant_pool = NULL;
    svm->global_variable_count = 0;
    svm->global_variables = NULL;
    svm->global_variable_types = NULL;
    svm->code_size = 0;
    svm->code = NULL;
    svm->function_count = 0;
    svm->functions = NULL;

    svm->stack_size = 0;
    svm->stack = NULL;
    svm->pc = 0;
    svm->sp = 0;
    return svm;
}

static void parse_header(uint8_t **p) {
    for (int i = 0; i < 7; ++i, (*p)++) {
        printf("%c ", (char)(**p));
    }
    printf("\n");
}

static int read_int(uint8_t **p) {
    uint8_t v1 = **p; (*p)++;
    uint8_t v2 = **p; (*p)++;
    uint8_t v3 = **p; (*p)++;
    uint8_t v4 = **p; (*p)++;        
    int v =(int)( v1 << 24 | v2 << 16 | v3 << 8 | v4);
    return v;
}

static short read_short(uint8_t **p) {
    uint8_t v1 = **p; (*p)++;
    uint8_t v2 = **p; (*p)++;
    short v = (short)(v1 << 8 | v2);
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

static void create_functions(SVM_VirtualMachine* svm) {
    svm->functions = (SVM_Function*)MEM_malloc(sizeof(SVM_Function) * svm->function_count);
    for (int i = 0; i < svm->function_count; ++i) {
        memset(&svm->functions[i], 0, sizeof(SVM_Function));
    }
    /*
    if (svm->functions[0].u.native_f.name == NULL) {
        printf("NULL\n");
    }
    */
}

static void create_types(SVM_Function* svm_f) {
    if (svm_f->param_count > 0) 
        svm_f->param_type = MEM_malloc(sizeof(int) * svm_f->param_count);

    if (svm_f->local_variable_count > 0)
        svm_f->local_variable_type = MEM_malloc(sizeof(int) * svm_f->local_variable_count);    
}

static void delete_functions(SVM_VirtualMachine* svm) {
    if (!svm->functions) return;

    for (int i = 0; i < svm->function_count; ++i) {        
        if (svm->functions[i].param_type) MEM_free(svm->functions[i].param_type);
        if (svm->functions[i].local_variable_type) MEM_free(svm->functions[i].local_variable_type);

        if (svm->functions[i].type == NATIVE_FUNCTION) {
            if (svm->functions[i].u.native_f.name)
                MEM_free(svm->functions[i].u.native_f.name);
        }
    }
    MEM_free(svm->functions);
}


static void parse_binary(SVM_VirtualMachine* svm, uint8_t* buf) {
    uint8_t *pos = buf;

    /* parse header */
    parse_header(&pos);    

    /* parse constant pool */
    int constant_pool_size = read_int(&pos);
    printf("constant pool size = %d\n", constant_pool_size);
    svm->constant_pool_count = constant_pool_size;
    svm->constant_pool = (SVM_Constant*)MEM_malloc(sizeof(SVM_Constant) * constant_pool_size);

    for (int i = 0; i < constant_pool_size; ++i) {
        uint8_t c_type = read_byte(&pos);
        svm->constant_pool[i].type = c_type;
        switch (c_type) {
            case SVM_INT: {
                int iv = read_int(&pos);
                svm->constant_pool[i].u.c_int = iv;
                break;
            }
            case SVM_DOUBLE: {
                double dv = read_double(&pos);
                svm->constant_pool[i].u.c_double = dv;
                break;
            }
            default: {
                break;
            }
        }
    }

    /* parse functions */
    int function_count = read_int(&pos);
    svm->function_count = function_count;
    printf("function count = %d\n", svm->function_count);
    create_functions(svm);

    for (int i = 0; i < svm->function_count; ++i) {
        int parameter_count = read_int(&pos);
        int local_variable_count = read_int(&pos);
        svm->functions[i].param_count = parameter_count;
        svm->functions[i].local_variable_count = local_variable_count;
        create_types(&svm->functions[i]);
        for (int j = 0; j < svm->functions[i].param_count; ++j) {
            int type = read_int(&pos);
            svm->functions[i].param_type[j] = type;
        }
        for (int j = 0; j < svm->functions[i].local_variable_count; ++j) {
            int type = read_int(&pos);
            svm->functions[i].local_variable_type[j] = type;
        }
    }

    /* parse global variables */
    int global_variable_count = read_int(&pos);
    printf("global variable count = %d\n", global_variable_count);
    svm->global_variable_count = global_variable_count;
    if (svm->global_variable_count > 0) {
        svm->global_variables = (SVM_Value*)MEM_malloc(sizeof(SVM_Value) * svm->global_variable_count);
        svm->global_variable_types = (int*)MEM_malloc(sizeof(int) * svm->global_variable_count);
        for (int i = 0; i < svm->global_variable_count; ++i) {
            int type = read_int(&pos);
            svm->global_variable_types[i] = type;            
        }
    }

    /* parse function again */
    for (int i = 0; i < svm->function_count; ++i) {
        uint16_t index = read_short(&pos);
        printf("index = %04x\n", index);
        SVM_Boolean is_implemented = read_byte(&pos);
        if (is_implemented) {
            uint16_t f_addr = read_short(&pos);
            printf("addr = %02x\n", f_addr);
        } else {
            uint8_t nlen = (uint8_t)read_byte(&pos);
            svm->functions[i].u.native_f.nlen = nlen;
            svm->functions[i].u.native_f.name = (char*)MEM_malloc(nlen + 1);
            for (int si = 0; si < nlen; ++si) {
                svm->functions[i].u.native_f.name[si] = read_byte(&pos);
            }
            svm->functions[i].u.native_f.name[nlen] = 0;
            printf("fname = %s\n", svm->functions[i].u.native_f.name);
        }
    }

    int total_code_size = read_int(&pos);
    uint16_t entry_address = read_short(&pos);

    svm->code_size = total_code_size;
    svm->pc = entry_address;

    svm->code = (uint8_t*)MEM_malloc(total_code_size);
    memcpy(svm->code, pos, total_code_size);

    /*
    for (int i = 0; i < total_code_size; ++i) {
        if (i % 16 == 0) printf("\n");
        printf("%02x ", svm->code[i]);
    }
    */

}

static void svm_delete(SVM_VirtualMachine* svm) {
    if (!svm) return;

    if (svm->constant_pool) MEM_free(svm->constant_pool);
    if (svm->global_variables) MEM_free(svm->global_variables);
    if (svm->global_variable_types) MEM_free(svm->global_variable_types);
    if (svm->code) MEM_free(svm->code);
    if (svm->stack) MEM_free(svm->stack);
    delete_functions(svm);

    MEM_free(svm);
}

static void svm_init(SVM_VirtualMachine* svm) {
    svm->stack_size = 100; // temporary
    svm->stack = (SVM_Value*)MEM_malloc(sizeof(SVM_Value) * svm->stack_size);
    svm->sp = 0;

    for (int i = 0; i < svm->global_variable_count; ++i) {
        switch (svm->global_variable_types[i]) {
            case SVM_INT: {
                svm->global_variables[i].ival = 0;
                break;
            }
            case SVM_DOUBLE: {
                svm->global_variables[i].dval = 0.0;
                break;
            }
            default: {
                fprintf(stderr, "no such svm type\n");
                exit(1);
            }
        }
    }
}



int main(int argc, char* argv[]) {

    struct stat st;
    uint8_t* buf;
    char* fname;
    int fd;
    int size;

    printf("svm\n");

    fname = argv[argc - 1];    
    int status = stat(fname, &st);
    //buf = (char*)malloc(st.st_size);
    buf = (uint8_t*)MEM_malloc(st.st_size);
    
    fd = open(fname, O_RDONLY);
    size = read(fd, buf, st.st_size);
    close(fd);

    SVM_VirtualMachine* svm = svm_create();
    parse_binary(svm, buf);
    
    svm_init(svm);

    //disasm(svm->code, svm->code_size);



    MEM_free(buf);

    svm_delete(svm);

    MEM_dump_memory();





    return 0;
}