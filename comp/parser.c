#include <stdio.h>
#include <stdlib.h>
#include "scantest.h"

#define ERR -1
#define OK 1

extern FILE *yyin;
extern int yylex();

static int t_type;

static int EFLG = OK;

static int depth = 0;

static void increment() {
    depth++;
}

static void decrement() {
    depth--;
}

static void debug(const char* str) {
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("%s\n", str);
}

static void expression();

static void factor() {
    increment();
    debug("factor");
    char buf[10];

    switch (t_type) {
        case INT_LITERAL: {
            t_type = yylex();
	    sprintf(buf, ":%d", yylval.iv);
	    debug(buf);
            break;
        }
        case LP: {
	    debug("(");
            t_type = yylex();	    
            expression();
            if (t_type == RP) {
		debug(")");
                t_type = yylex();
            } else {
                EFLG = ERR;
            }
            break;
        }
        default: {            
            EFLG = ERR;
            break;
        }
    }
    decrement();    
}


static void term_d() {
    if (t_type == MUL || t_type == DIV) {
	debug("* | /");
	t_type = yylex();
	factor();
	term_d();
    }
}

static void term() {
    increment();
    debug("term");
    factor();
    term_d();
    decrement();
}

static void expression_d() {
    if (t_type == ADD || t_type == SUB) {
        debug("+ | -");
	t_type = yylex();
	term();
	expression_d();
    }
}

static void expression() {
    increment();
    debug("expression");
    term();
    expression_d();
    decrement();		
}

static void statement() {
    debug("statement");
    expression();
    if (t_type == SEMICOLON) {
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
    
    t_type = yylex();
    statement();

    if (EFLG == ERR) {
        printf("parse error\n");
    } else {
        printf("parse success\n");
    }
    
}
