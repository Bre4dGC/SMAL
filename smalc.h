#pragma once

#include <stdbool.h>

/*################# LEXER #################*/

typedef enum {
    T_EOF,      // \0
    T_EOL,      // \n
    T_VAR,      // a..z
    T_NUM,      // 0..9
    T_ASSIGN,   // a = 5
    T_ADD,      // a + b
    T_SUB,      // b - a
    T_LANGLE,   // <
    T_RANGLE,   // >
    T_COND,     // ?
    T_LOOP,     // @
} token_kind_t;

typedef struct {
    token_kind_t kind;
    union {
        char ident; // single character variable identificator
        int number; // just a number
    };
} token_t;

void tokenize(char* code);

typedef struct node node_t;

typedef struct {
    char ident;
    int  value;
} ref_t;

typedef struct {
    ref_t variables[26];
    int count;
} decl_t;

typedef struct {
    enum {C_LT, C_GT} cond;
    enum {C_COND, STMT_LOOP} type;
} stmt_t;

void parse(void);
void execute(char* src);