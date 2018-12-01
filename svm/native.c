#include <stdio.h>
#include "../memory/MEM.h"
#include "svm.h"

static SVM_Value native_print(SVM_VirtualMachine* svm, SVM_Value* values, int arg_count) {
    SVM_Value v;
    v.ival = 0;
    printf("%d\n", values[0].ival);
    printf("%f\n", values[1].dval);    
    return v;    
}

void add_native_functions(SVM_VirtualMachine* svm) {
    svm_add_native_function(svm, native_print, "print", 2);
}






