#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../memory/MEM.h"
#include "svm.h"

#define revalue_up_align(val)   ((val) ? (((val) - 1) / sizeof(SVM_Value) + 1) : 0)
#define CALLINFO_SIZE revalue_up_align(sizeof(CallInfo))

static bool use_stack(uint8_t op) {
    switch (op) {
        case SVM_PUSH_STACK_INT:
        case SVM_PUSH_STACK_DOUBLE:
        case SVM_POP_STACK_INT:
        case SVM_POP_STACK_DOUBLE: {
            return true;
        }            
        default: {
            return false;
        }
    }    
}

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

    svm->ci_offset = 0;
    svm->base = 0;
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
                printf("%d: %d\n", i, iv);
                break;
            }
            case SVM_DOUBLE: {
                double dv = read_double(&pos);
                svm->constant_pool[i].u.c_double = dv;
                printf("%d: %f\n", i,svm->constant_pool[i].u.c_double);
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
            svm->functions[i].type = SVM_FUNCTION;
            uint16_t f_addr = read_short(&pos);
            svm->functions[i].u.svm_f.f_addr = f_addr;
            printf("addr = %02x\n", f_addr);
            printf("addr = %02x\n", svm->functions[i].u.svm_f.f_addr);

        } else {
            svm->functions[i].type = NATIVE_FUNCTION;
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


    for (int i = 0; i < total_code_size; ++i) {
        if (i % 16 == 0) printf("\n");
        printf("%02x ", svm->code[i]);
    }


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
    svm->base = 0;
    svm->ci_offset = 0;

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


static uint8_t fetch(SVM_VirtualMachine* svm) {
    return svm->code[svm->pc++];
}

static uint16_t fetch2(SVM_VirtualMachine* svm) {
    uint8_t v1 = fetch(svm);
    return (v1 << 8) | fetch(svm);
}

/* stack operations */
static void push_i(SVM_VirtualMachine* svm, int iv) {
    svm->stack[svm->sp++].ival = iv;
}

static void push_d(SVM_VirtualMachine* svm, double dv) {
    //fprintf(stderr, "push double: %f", dv);
    svm->stack[svm->sp++].dval = dv;
}

static double pop_d(SVM_VirtualMachine* svm) {
    return svm->stack[--svm->sp].dval;
}

static int pop_i(SVM_VirtualMachine* svm) {
    return svm->stack[--svm->sp].ival;
}

static SVM_Value* pop(SVM_VirtualMachine* svm) {
    return &svm->stack[--svm->sp];
}

static SVM_Constant* get_constant(SVM_VirtualMachine* svm, uint16_t idx) {
    return &svm->constant_pool[idx];    
}

static double read_constant_double(SVM_VirtualMachine* svm, uint16_t idx) {
    return get_constant(svm, idx)->u.c_double;    
}

static int read_constant_int(SVM_VirtualMachine* svm, uint16_t idx) {
    return get_constant(svm, idx)->u.c_int;
}

static int get_param_number(SVM_VirtualMachine* svm, uint16_t idx) {

    int next_func_idx = -1;
    int target_func_idx = -1;
    for (int i = 0; i < svm->function_count; ++i) {
        if (svm->functions[i].type == SVM_FUNCTION) {
            if (svm->functions[i].u.svm_f.f_addr > idx) {
                next_func_idx = i;
                break;
            }
        }
    }
    /* target is the last function*/
    if (next_func_idx == -1) {
        next_func_idx = svm->function_count - 1;
    }

    //printf("next_func_idx = %d\n", next_func_idx);

    for (int i = next_func_idx; i >= 0; --i) {
        if (svm->functions[i].type == NATIVE_FUNCTION) {            
            continue;
        }
        //printf("f_addr = %02x\n", svm->functions[i].u.svm_f.f_addr);
        if (svm->functions[i].u.svm_f.f_addr <= idx) {
            target_func_idx = i;
            break;
        }
    }

    /* Error */
    if (target_func_idx == -1) {
        fprintf(stderr, "cannot find function in get_param_number\n");
        exit(1);
    }

    //printf("target_func_idx = %d\n", target_func_idx);

    return svm->functions[target_func_idx].param_count;
}

static void adjust_stack_address(SVM_VirtualMachine* svm) {
    printf("-- adjust stack address --\n");    
    for (int i = 0; i < svm->code_size; ++i) {
        SVM_Opcode op = svm->code[i];        
        OpcodeInfo oInfo = svm_opcode_info[op];
        for (int pidx = 0; pidx < strlen(oInfo.parameter); ++pidx) {
            switch (oInfo.parameter[pidx]) {
                case 'i': {
                    if (use_stack(op)) {
                        //printf("(%02x)op = %02x\n", i, op);
                        uint8_t v1 = svm->code[i+1];
                        uint8_t v2 = svm->code[i+2];
                        uint16_t idx = (uint16_t)(v1 << 8 | v2);
                        //printf("idx = %04x\n", idx);

                        int param_count = get_param_number(svm, i);
                        //printf("param_count = %d\n", param_count);

                        // if local variables
                        if (idx + 1 > param_count) {
                            // update index
                            //printf("---> update index\n");
                            idx += CALLINFO_SIZE;
                            //printf("new idx = %04x\n", idx);
                            svm->code[i+1] = (idx >> 8) & 0xff;
                            svm->code[i+2] = (idx >> 0) & 0xff;
                        }
                    }
                    i += 2;
                    break;
                }
                default: {
                    fprintf(stderr, "");
                    exit(1);
                }
            }
        }
    }
}

static void svm_run(SVM_VirtualMachine* svm) {
    printf("---- execute ----\n");
    bool running = true;

    uint8_t op = 0;
    while (running) {
        op = fetch(svm);
        printf("%02x : %s\n", op, svm_opcode_info[op].opname);

        switch (op) {
            case SVM_PUSH_INT: {
                uint16_t c_idx = fetch2(svm);
                int i_val = read_constant_int(svm, c_idx);
                //printf("i_val = %d\n", i_val);
                push_i(svm, i_val);
                //printf("sp = %d\n", svm->sp);
                break;
            }
            case SVM_PUSH_DOUBLE: {
                uint16_t c_idx = fetch2(svm);
                //fprintf(stderr, "%04x\n", c_idx);
                double d_val = read_constant_double(svm, c_idx);                
                push_d(svm, d_val);
                //fprintf(stderr, "%f\n", d_val); 
                break;
            }
            case SVM_PUSH_STACK_INT: {
                uint16_t s_idx = fetch2(svm);
                //printf("s_idx = %d\n", s_idx);
                int i_val = svm->stack[s_idx + svm->base].ival;
                //printf("i_val = %d\n", i_val);
                push_i(svm, i_val);
                break;
            }
            case SVM_POP_STACK_INT: {
                uint16_t s_idx = fetch2(svm);
                //printf("s_val = %d\n", s_idx);
                int i_val = pop_i(svm);
                //printf("i_val = %d\n", i_val);
                //printf("sp = %d\n", svm->sp);
                svm->stack[svm->base + s_idx].ival = i_val;
                
                //exit(1);
                break;
            }
            case SVM_ADD_INT: {
                int v1 = pop_i(svm);
                int v2 = pop_i(svm);
                //printf("v1:v2 = %d:%d\n", v1, v2);
                push_i(svm, v1 + v2);
                break;
            }
            case SVM_CAST_INT_TO_DOUBLE: {
                int i_val = pop_i(svm);
                //printf("i_val = %d\n", i_val);
                push_i(svm, (int)i_val);
                break;
            }
            case SVM_CAST_DOUBLE_TO_INT: {
                //fprintf(stderr, "cast double to int\n");
                double d_val = pop_d(svm);
                push_i(svm, (int)d_val);
                break;
            }
            case SVM_POP: {
                SVM_Value* pv = pop(svm);
                printf("ival = %d\n", pv->ival);
                printf("sp   = %d\n", svm->sp);
                exit(1);
                break;
            }
            case SVM_PUSH_FUNCTION: {
                uint16_t f_idx = fetch2(svm);
                push_i(svm, f_idx);
                break;
            }
            case SVM_INVOKE: { // difficult                
                int f_idx = pop_i(svm);
                //printf("f_idx = %d\n", f_idx);
                SVM_Function* func = &svm->functions[f_idx];
                if (func->type == SVM_FUNCTION) {
                    //printf("svm function\n");
                    //printf("sp = %d\n", svm->sp);
                    CallInfo* cInfo = (CallInfo*)&svm->stack[svm->sp];
                    //CallInfo* cInfo = (CallInfo*)MEM_malloc(sizeof(CallInfo));
                    cInfo->ret_pc = svm->pc;
                    cInfo->prev_base = svm->base;
                    cInfo->f_idx = f_idx;
                    cInfo->prev_ci_offset = svm->ci_offset;

                    //printf("ret_pc = %02x\n", cInfo->ret_pc);
                    //printf("param_count = %d\n", func->param_count);
                    svm->base = svm->sp - func->param_count;
                    svm->ci_offset = svm->sp - svm->base;
                    //printf("ci_offset = %d\n", svm->ci_offset);

                    //printf("base = %d\n", svm->base);
                    //printf("prev_base = %d\n", cInfo->prev_base);
                    
                    //printf("callinfo size = %ld\n", CALLINFO_SIZE);
                    //printf("local_variable_size = %d\n", func->local_variable_count);
                    svm->sp += CALLINFO_SIZE + func->local_variable_count;

                    //printf("sp = %d\n", svm->sp);
                    svm->pc = func->u.svm_f.f_addr;
                    //printf("fpc = %02x\n", svm->pc);
                    

                } else if (func->type == NATIVE_FUNCTION) {
                    printf("native function\n");
                } else {
                    printf("invalid function type\n");
                    exit(1);
                }
                //printf("end of invoke\n");                
                break;
            }
            case SVM_RETURN: {
                SVM_Value ret_value = svm->stack[svm->sp-1];
                //printf("ret_value = %d\n", ret_value.ival);
                CallInfo* cInfo = (CallInfo*)&svm->stack[svm->base + svm->ci_offset];
                //printf("ret_pc = %02x\n", cInfo->ret_pc);
                svm->sp = svm->base;
                svm->pc = cInfo->ret_pc;
                svm->base = cInfo->prev_base;
                svm->ci_offset = cInfo->prev_ci_offset;
                svm->stack[svm->sp++] = ret_value;

                //printf("svm->sp = %d\n", svm->sp);
                //printf("ci_offset = %d\n", svm->ci_offset);

                break;
            }
            default: {
                fprintf(stderr, "unrecognized operator %02x\n", op);
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
    disasm(svm->code, svm->code_size);
    //printf("align size = %ld\n", CALLINFO_SIZE);
    adjust_stack_address(svm);

    printf("-- rewritten byte code --\n");
    disasm(svm->code, svm->code_size);

    svm_run(svm);


    MEM_free(buf);

    svm_delete(svm);

    MEM_dump_memory();





    return 0;
}