#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define VARIABLES_LIMIT 26
#define ARRAY_LIMIT 128

/*################# LEXER #################*/

typedef enum {
    T_EOF,  // \0
    T_EOL,  // \n
    T_TAB,  // \t
    T_VAR,  // a..z
    T_NUM,  // 0..9
    T_MUL    = '*',
    T_DIV    = '/',
    T_ADD    = '+',
    T_SUB    = '-',
    T_ASSIGN = '=',
    T_LANGLE = '<',
    T_RANGLE = '>',
    T_EQUAL  = ':',
    T_NEQUAL = '!',
    T_RAND   = '~',
    T_INDEX  = '"',
    T_COND   = '?',
    T_LOOP   = '@',
} token_kind_t;

typedef struct {
    token_kind_t kind;
    union {
        char ident;     // single character variable identificator
        size_t number;  // just a number
    };
} token_t;

typedef struct {
    char ident;
    size_t value[ARRAY_LIMIT];
    size_t count;
    int scope;
} ref_t;

typedef struct {
    ref_t variables[VARIABLES_LIMIT];
    int count;
} decl_t;

typedef struct {
    bool cond;
    size_t left, right;
    int jump_to, loop_back;
    enum {STMT_COND = T_COND, STMT_LOOP = T_LOOP} type;
} stmt_t;

void error(char* msg);
void tokenize(char* code);
void parse(void);
void execute(char* src);
void repl(void);
