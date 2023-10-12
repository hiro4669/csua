#ifndef _DLEX_H_
#define _DLEX_H_

typedef union {
    int iv;
    double dv;
    char *name;
} YYSTYPE;

YYSTYPE yylval;

#endif
