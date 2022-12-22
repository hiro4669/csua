/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LP = 258,
    RP = 259,
    LC = 260,
    RC = 261,
    COMMA = 262,
    LOGICAL_AND = 263,
    LOGICAL_OR = 264,
    EQ = 265,
    ASSIGN_T = 266,
    NE = 267,
    GT = 268,
    GE = 269,
    LE = 270,
    LT = 271,
    SEMICOLON = 272,
    COLON = 273,
    ADD = 274,
    SUB = 275,
    MUL = 276,
    DIV = 277,
    MOD = 278,
    ADD_ASSIGN_T = 279,
    SUB_ASSIGN_T = 280,
    MUL_ASSIGN_T = 281,
    DIV_ASSIGN_T = 282,
    MOD_ASSIGN_T = 283,
    INCREMENT = 284,
    DECREMENT = 285,
    EXCLAMATION = 286,
    DOT = 287,
    INT_LITERAL = 288,
    DOUBLE_LITERAL = 289,
    STRING_LITERAL = 290,
    IDENTIFIER = 291,
    IF = 292,
    ELSE = 293,
    ELSIF = 294,
    WHILE = 295,
    FOR = 296,
    RETURN = 297,
    BREAK = 298,
    CONTINUE = 299,
    TRUE_T = 300,
    FALSE_T = 301,
    BOOLEAN_T = 302,
    INT_T = 303,
    DOUBLE_T = 304,
    STRING_T = 305
  };
#endif
/* Tokens.  */
#define LP 258
#define RP 259
#define LC 260
#define RC 261
#define COMMA 262
#define LOGICAL_AND 263
#define LOGICAL_OR 264
#define EQ 265
#define ASSIGN_T 266
#define NE 267
#define GT 268
#define GE 269
#define LE 270
#define LT 271
#define SEMICOLON 272
#define COLON 273
#define ADD 274
#define SUB 275
#define MUL 276
#define DIV 277
#define MOD 278
#define ADD_ASSIGN_T 279
#define SUB_ASSIGN_T 280
#define MUL_ASSIGN_T 281
#define DIV_ASSIGN_T 282
#define MOD_ASSIGN_T 283
#define INCREMENT 284
#define DECREMENT 285
#define EXCLAMATION 286
#define DOT 287
#define INT_LITERAL 288
#define DOUBLE_LITERAL 289
#define STRING_LITERAL 290
#define IDENTIFIER 291
#define IF 292
#define ELSE 293
#define ELSIF 294
#define WHILE 295
#define FOR 296
#define RETURN 297
#define BREAK 298
#define CONTINUE 299
#define TRUE_T 300
#define FALSE_T 301
#define BOOLEAN_T 302
#define INT_T 303
#define DOUBLE_T 304
#define STRING_T 305

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 9 "csua.y"

    int                  iv;
    double               dv;
    char                *str;
    char                *name;
    Expression          *expression;
    Statement           *statement;
    FunctionDeclaration *function_declaration;
    AssignmentOperator   assignment_operator;
    CS_BasicType         type_specifier;

#line 169 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
