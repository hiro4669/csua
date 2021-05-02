#include <stdio.h>
#include <stdlib.h>

#include "csua.h"

int main(void) {


    extern int yyparse(void);
    extern FILE *yyin;
    yyin = fopen("tests/prog1.cs", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }    
    CS_Compiler* compiler = CS_create_compiler(); // this is required
    if (yyparse()) {
        fprintf(stderr, "Parse Error\n");
        exit(1);
    }
    

    fclose(yyin);
    return 0;
}