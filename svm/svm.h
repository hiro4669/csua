
#ifndef _SVM_H_
#define _SVM_H_

typedef enum {
    SVM_PUSH_INT = 1,
    SVM_PUSH_DOUBLE,
    SVM_PUSH_STACK_INT,
    SVM_PUSH_STACK_DOUBLE,
    SVM_POP_STACK_INT,
    SVM_POP_STACK_DOUBLE,
    SVM_PUSH_STATIC_INT,
    SVM_PUSH_STATIC_DOUBLE,
    SVM_POP_STATIC_INT,
    SVM_POP_STATIC_DOUBLE,
    SVM_ADD_INT,
    SVM_ADD_DOUBLE,
    SVM_SUB_INT,
    SVM_SUB_DOUBLE,
    SVM_MUL_INT,
    SVM_MUL_DOUBLE,
    SVM_DIV_INT,
    SVM_DIV_DOUBLE,
    SVM_MOD_INT,
    SVM_MOD_DOUBLE,
    SVM_MINUS_INT,
    SVM_MINUS_DOUBLE,
    SVM_INCREMENT,
    SVM_DECREMENT,
    SVM_CAST_INT_TO_DOUBLE,
    SVM_CAST_DOUBLE_TO_INT,
    SVM_EQ_INT,
    SVM_EQ_DOUBLE,
    SVM_NE_INT,
    SVM_NE_DOUBLE,
    SVM_GT_INT,
    SVM_GT_DOUBLE,
    SVM_GE_INT,
    SVM_GE_DOUBLE,
    SVM_LT_INT,
    SVM_LT_DOUBLE,
    SVM_LE_INT,
    SVM_LE_DOUBLE,
    SVM_LOGICAL_AND,
    SVM_LOGICAL_OR,
    SVM_LOGICAL_NOT,
    SVM_POP,
    SVM_PUSH_FUNCTION,
    SVM_INVOKE,
    SVM_RETURN
} SVM_Opcode;

typedef struct {
    char    *opname;
    char    *parameter;    
    uint8_t s_size;
} OpcodeInfo;

OpcodeInfo svm_opcode_info[] = {
    {"dummy", "", 0},
    {"push_int", "i", 1},
    {"push_double", "i", 1},
    {"push_stack_int", "i", 1},
    {"push_stack_double", "i", 1},

    {"pop_stack_int", "i", -1},
    {"pop_stack_double", "i", -1},
    
    {"push_static_int", "i", 1},
    {"push_static_double", "i", 1},
    
    {"pop_static_int", "i", -1},
    {"pop_static_double", "i", -1},
    
    {"add_int", "", -1}, 
    {"add_double", "", -1},
    {"sub_int", "", -1}, 
    {"sub_double", "", -1},
    {"mul_int", "", -1}, 
    {"mul_double", "", -1},
    {"div_int", "", -1}, 
    {"div_double", "", -1},
    {"mod_int", "", -1}, 
    {"mod_double", "", -1},
    {"minus_int", "", 0}, 
    {"minus_double", "", 0},   
    {"increment", "", 0},
    {"decrement", "", 0},
    {"cast_int_to_double", "", 0},
    {"cast_double_to_int", "", 0},
    {"eq_int", "", -1},
    {"eq_double", "", -1}, 
    {"ne_int", "", -1},    
    {"ne_double", "", -1},
    {"gt_int", "", -1},
    {"gt_double", "", -1},
    {"ge_int", "", -1},
    {"ge_double", "", -1},
    {"lt_int", "", -1},
    {"lt_double", "", -1},
    {"le_int", "", -1},    
    {"le_double", "", -1},
    {"logical_and", "", -1},
    {"logical_or", "", -1},
    {"logical_not", "", -1},
    {"pop", "", -1},    
    {"push_function", "", 0},
    {"invoke", "", -1},
    {"return", "", -1},
            
};



#endif