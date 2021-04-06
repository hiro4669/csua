#ifndef _SVM_H
#define _SVM_H

#include <stdint.h>

typedef struct CS_TypeSpecifier_tag CS_TypeSpecifier;

typedef enum {
    CS_FALSE = 0,
    CS_TRUE = 1    
} CS_Boolean;

typedef enum {
    CS_BOOLEAN_TYPE,
    CS_INT_TYPE,
    CS_DOUBLE_TYPE,    
} CS_BasicType;


typedef struct {
    char          *name;
    CS_TypeSpecifier *type;
} CS_Variable;

typedef enum {
    CS_CONSTANT_INT,
    CS_CONSTANT_DOUBLE
} CS_ConstantType;

typedef struct {
    CS_ConstantType type;
    union {
      int    c_int;
      double c_double;
    }u;    
} CS_ConstantPool;


struct CS_TypeSpecifier_tag {
    CS_BasicType       basic_type;
};

typedef struct {
    uint32_t         constant_pool_count;
    CS_ConstantPool *constant_pool;
    uint32_t         global_variable_count;
    CS_Variable     *global_variable;
    uint32_t        code_size;
    uint8_t         *code;
} CS_Executable;


#endif