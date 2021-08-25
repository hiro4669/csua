#ifndef _SCANTEST_H_
#define _SCANTEST_H_


#define INT_LITERAL    100
#define DOUBLE_LITERAL 101
#define LP             102
#define RP             103
#define LC             104
#define RC             105
#define EQ             109
#define SEMICOLON      116
#define COLON          117
#define ADD            118
#define SUB            119
#define MUL            120
#define DIV            121
#define MOD            122



typedef union {
    int iv;
    double dv;
    char *name;
} YYSTYPE;

YYSTYPE yylval;

#endif
