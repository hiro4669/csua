#include <stdio.h>
#include "svm.h"

//int print(int arg) {
SVM_Value print(SVM_VirtualMachine* svm, SVM_Value* values, int arg_count) {    
//    printf("arg_count = %d\n", arg_count);
    printf("%d\n", values[arg_count-1].ival);
    SVM_Value v;
    v.ival = 1;
    return v;
}