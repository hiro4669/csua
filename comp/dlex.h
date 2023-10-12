#ifndef _DLEX_H_
#define _DLEX_H_

typedef union {
    int iv;
    double dv;
    char *name;
} YYSTYPE;

extern YYSTYPE yylval;

#endif
