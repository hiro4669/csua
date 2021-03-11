#include <stdio.h>
#include <stdlib.h>
#include "scantest.h"

#define ERR -1
#define OK 1

extern FILE *yyin;
extern int yylex();

static int t_type;

static int EFLG = OK;

static void token_test() {    

    while (1) {
        t_type = yylex();
        switch (t_type) {
            case SEMICOLON: {
                printf("SEMICOLON(;)\n");
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
            case LP: {
                printf("(\n");
                break;
            }
            case RP: {
                printf(")\n");
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

static void expression();

static void factor() {
    printf("factor\n");

    switch (t_type) {
        case INT_LITERAL: {
            t_type = yylex();
            break;
        }
        case LP: {
            t_type = yylex();
            expression();
            if (t_type == RP) {
                t_type = yylex();
            } else {
                EFLG = ERR;
            }
            break;
        }
        default: {
            printf("hoge?\n");
            EFLG = ERR;
            break;
        }
    }

    /*
    if (t_type == INT_LITERAL) {        
        t_type = yylex();
        return;
    }
    */
    
}

static void term() {
    printf("term\n");
    factor();
    if (t_type == MUL | t_type == DIV) {
        printf("* | /\n");
        t_type = yylex();
        factor();
    }    
}

static void expression() {
    printf("expression\n");
    term();
    if (t_type == ADD | t_type == DIV) {
        printf("+ | -\n");
        t_type = yylex();
        term();
    }
}


static void statement() {
    printf("statement\n");
    expression();
    //t_type = yylex();
    if (t_type == SEMICOLON) {
        //printf("semicolon\n");
        return;
    } 
    t_type = yylex();
    if (t_type == EOF) {
        printf("end of input\n");
        exit(1);
    }
    
    EFLG = ERR;
}


int main(void) {
    
    yyin = fopen("tests/calc.cs", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }
    printf("Hello World\n");
    //token_test();
    
    t_type = yylex();
    statement();

    if (EFLG == ERR) {
        printf("parse error\n");
    } else {
        printf("parse success\n");
    }
    
}