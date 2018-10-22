
#ifndef _SVM_H_
#define _SVM_H_

typedef enum {
    SVM_PUSH_INT = 1,
    SVM_PUSH_DOUBLE,
    SVM_PUSH_STACK_INT,
    SVM_PUSH_STACK_DOUBLE,
    SVM_PUSH_STATIC_INT,
    SVM_PUSH_STATIC_DOUBLE,
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
    SVM_PUSH_FUNCTION
    SVM_INVOKE,
    SVM_RETURN
} SVM_Opcode;

typedef struct {
    char *opname;
    char *parameter;    
} OpcodeInfo;

OpcodeInfo svm_opcode_info[] = {
    {"dummy", ""},
    {"push_int", ""},
    {"push_double", ""},
    {"push_stack_int", ""},
    {"push_stack_double", ""},
    {"push_static_int", ""},
    {"push_static_double", ""},
    {"add_int", ""}, 
    {"add_double", ""},
    {"sub_int", ""}, 
    {"sub_double", ""},
    {"mul_int", ""}, 
    {"mul_double", ""},
    {"div_int", ""}, 
    {"div_double", ""},
    {"mod_int", ""}, 
    {"mod_double", ""},
    {"minus_int", ""}, 
    {"minus_double", ""},   
    {"increment", ""},
    {"decrement", ""},
    {"cast_int_to_double", ""},
    {"cast_double_to_int", ""},
    {"eq_int", ""},
    {"eq_double", ""}, 
    {"ne_int", ""},    
    {"ne_double", ""},
    {"gt_int", ""},
    {"gt_double", ""},
    {"ge_int", ""},
    {"ge_double", ""},
    {"lt_int", ""},
    {"lt_double", ""},
    {"le_int", ""},    
    {"le_double", ""},
    {"logical_and", ""},
    {"logical_or", ""},
    {"logical_not", ""},
    {"pop", ""},    
    {"push_function", ""},
    {"invoke", ""},
    {"return", ""},
            
};



#endif