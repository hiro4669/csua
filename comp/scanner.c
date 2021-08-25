#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "keyword.h"

#include "scantest.h"


#define ISASCII(c) isascii((unsigned char)(c))
#define ISALNUM(c) (ISASCII(c) && isalnum((unsigned char)(c)))
#define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#define is_identchar(c) (SIGN_EXTEND_CHAR(c)!=-1&&(ISALNUM(c) || (c) == '_'))

extern struct OPE *in_word_set(char*, unsigned int);

#define BUFSIZE 5

FILE *yyin = NULL;

static uint8_t *buffer = NULL;
static uint32_t ptr = 0;
static uint32_t limit = 0;

char *yytext;
static uint32_t ytp = 0;
static uint32_t yt_max = 0;

static void real_read() {
    if (buffer == NULL) {
        buffer = (uint8_t*)malloc(BUFSIZE);
    }
    
    if (yytext == NULL) {
        ytp = 0;
        yytext = (char*)malloc(BUFSIZE);
        yt_max += BUFSIZE;
    }
    
    ptr = 0;
    limit = fread(buffer, 1, BUFSIZE, yyin);
}

static uint8_t read() {
    if (ptr == limit) real_read();
    if (limit == 0) {
        return EOF;
    }
    return buffer[ptr++];
}

static void addText(char c) {
    if (ytp == (yt_max - 1)) {
        yt_max += BUFSIZE;
        yytext = (char*)realloc(yytext, yt_max);
    }
    yytext[ytp++] = c;
    yytext[ytp] = 0;
}

// test function
static void showText() {
    for (int i = 0; i < ytp; ++i) {
        printf("%d ", (int)yytext[i]);
    }
    printf("\n");
}

static void pushback() {
    --ptr;
}

static void error() {
    fprintf(stderr, "cannot understand character: %s\n", yytext);
    exit(1);
}

int yylex() {
    char c;

    ytp = 0;


retry:
    switch(c = read()) {
        case '#': { // skip comment
            while ((c = read()) != '\n');
            goto retry;
        }
        case ' ':
        case '\t': { // skip space and tab
            goto retry;
        }
        case '\n': { // ignore return and count the number
            goto retry;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':            
        case '5':
        case '6':            
        case '7':
        case '8':                        
        case '9': {
            addText(c);
            while(isdigit(c = read())) {
                addText(c);
            }
	    pushback();
	    int i_value;
	    sscanf(yytext, "%d", &i_value);
	    yylval.iv = i_value;
	    return INT_LITERAL;
        }
        case ';': {
            return SEMICOLON;
        }
        case '(': {
            return LP;
        }
        case ')': {
            return RP;
        }
        case '+': {
	    return ADD;
        }
        case '-': {
	    return SUB;
        }
        case '*': {
	    return MUL;
        }
        case '/': {
	    return DIV;
        }
        case EOF: {
            return EOF;
        }
        default: {
            break;
        }
    }
    return EOF;
}
