#include <stdio.h>
#include <stdlib.h>
#include "scantest.h"

#define ERR -1
#define OK 1

extern FILE *yyin;
extern int yylex();

static int t_type;

static void token_test() {    

    while (1) {
        t_type = yylex();
        switch (t_type) {
            case SEMICOLON: {
                printf("SEEMICOLON(;)\n");
                break;
            }
            case INT_LITERAL: {
                printf("INT: %d\n", yylval.iv);
                break;            
            }
            case ADD: {
                printf("ADD(+)\n");
                break;
            }
            case SUB: {
                printf("SUB(-)\n");
                break;
            }
            case MUL: {
                printf("MUL(*)\n");
                break;
            }
            case DIV: {
                printf("DIV(/)\n");
                break;
            }
            case EOF: {
                printf("END OF FILE\n");
                exit(1);
                break;
            }
        }
    }    
}

static int factor() {
    printf("factor\n");
    if (t_type == INT_LITERAL) {        
        t_type = yylex();
        return OK;
    }
    return ERR;
}

static int term() {
    factor();
    return OK;
}

static int expression() {
    term();
    return OK;
}


static int statement() {
    expression();
    //t_type = yylex();
    if (t_type == SEMICOLON) {
        printf("semicolon\n");
        return OK;
    } 
    t_type = yylex();
    if (t_type == EOF) {
        printf("end of input\n");
        exit(1);
    }
    return ERR;
}


int main(void) {
    
    yyin = fopen("tests/calc.cs", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }
    printf("Hello World\n");
    t_type = yylex();
    int r = statement();

    if (r == OK) {
        printf("parsing success\n");
    } else {
        printf("parsing error\n");
    }
    //token_test();
}