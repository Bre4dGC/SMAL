#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "smalc.h"

token_t* tokens;
decl_t decl;

void tokenize(char* code)
{
    tokens = calloc(256, sizeof(token_t));
    int count = 0;

    for(int i = 0; code[i] != '\0'; i++){
        switch(code[i]){
            case '=': tokens[count].kind = T_ASSIGN; break;
            case '+': tokens[count].kind = T_ADD; break;
            case '-': tokens[count].kind = T_SUB; break;
            case 'a'...'z':
                tokens[count].kind = T_VAR;
                tokens[count].ident = code[i];
                break;
            case '<':  tokens[count].kind = T_LANGLE; break;
            case '>':  tokens[count].kind = T_RANGLE; break;
            case '\n': tokens[count].kind = T_EOL; break;
            case '#':
                while(code[i] != '\n' && code[i] != '\0') i++; i--;
                continue;
            case ' ': case '\t': continue;
            default:
                if(isdigit(code[i])){
                    tokens[count].kind = T_NUM;
                    tokens[count].number = 0;
    
                    char str[8] = "";
                    int c = 0;
                    while(isdigit(code[i])) str[c++] = code[i++];
                    str[c] = '\0';
    
                    int num = 0;
                    for(int j = 0; j < strlen(str); j++) num = num * 10 + (str[j] - '0');
                    tokens[count].number = num;
                    i--;
                    break;
                }
                else fprintf(stderr, "Unknown token: %c\n", code[i]);
                continue;
        }
        count++;
        if(count >= 255) break;
    }
    tokens[count].kind = T_EOF;
}

void advance(token_t* curr, token_t* next, int* i)
{
    (*i)++;
    *curr = tokens[*i];
    *next = tokens[*i + 1];
}

int is_decl(char ident)
{
    for(int i = 0; i < decl.count; i++) if(decl.variables[i].ident == ident) return i;
    return -1;
}

static int get_token_value(token_t* t)
{
    if(t->kind == T_NUM) return t->number;
    if(t->kind == T_VAR){
        int idx = is_decl(t->ident);
        if(idx != -1) return decl.variables[idx].value;
        return 0;
    }
    return 0;
}

int parse_expr(token_t* current, token_t* next, int* start)
{
    int value = 0;
    int left = get_token_value(current);

    if(next->kind == T_EOL || next->kind == T_EOF){
        advance(current, next, start);
        return left;
    }

    token_t *right_tok = &tokens[(*start) + 2];
    int right = get_token_value(right_tok);

    switch(next->kind){
        case T_ADD: value = left + right; break;
        case T_SUB: value = left - right; break;
        default: value = left; break;
    }
    advance(current, next, start);
    advance(current, next, start);
    return value;
}

node_t* parse_stmt(int start)
{
    // SOON
    return NULL;
}

void parse(void)
{
    ref_t* ref = NULL;

    for(int i = 0; tokens[i].kind != T_EOF; i++){
        token_t current = tokens[i];
        token_t next = tokens[i+1];

        switch(current.kind){
            case T_VAR:
                switch(next.kind){
                    case T_VAR:
                        while(current.kind == T_VAR){
                            if(is_decl(current.ident) == -1){
                                if(decl.count < 26){
                                    decl.variables[decl.count] = (ref_t){current.ident, 0};
                                    decl.count++;
                                }
                            }
                            advance(&current, &next, &i);
                        }
                        break;
                    case T_ASSIGN: {
                        int idx = is_decl(current.ident);
                        if(idx == -1){
                            if(decl.count < 26){
                                decl.variables[decl.count] = (ref_t){current.ident, 0};
                                idx = decl.count;
                                decl.count++;
                            }
                            else idx = -1;
                        }
                        if(idx == -1){ fprintf(stderr, "Too many variables\n"); break; }
                        ref = &decl.variables[idx];
                        advance(&current, &next, &i);
                        advance(&current, &next, &i);
                        ref->value = parse_expr(&current, &next, &i);
                    } break;
                    case T_ADD: case T_SUB: {
                        int idx = is_decl(current.ident);
                        if(idx == -1){
                            if(decl.count < 26){
                                decl.variables[decl.count] = (ref_t){current.ident, 0};
                                idx = decl.count;
                                decl.count++;
                            }
                            else idx = -1;
                        }
                        if(idx == -1){ fprintf(stderr, "Too many variables\n"); break; }
                        ref = &decl.variables[idx];
                        advance(&current, &next, &i);
                        {
                            int rhs = get_token_value(&next);
                            if(current.kind == T_ADD) ref->value = ref->value + rhs;
                            else if(current.kind == T_SUB) ref->value = ref->value - rhs;
                        }
                    } break;
                    case T_LANGLE: case T_RANGLE:
                        // STATEMENTS SOON
                        break;
                    case T_EOL: case T_EOF: continue;
                    default:
                        fprintf(stderr, "Unexpected token: %d\n", current.kind);
                        continue;
                }
                break;
            case T_LANGLE: case T_RANGLE:
                token_kind_t stream = current.kind;
                advance(&current, &next, &i);

                switch(stream){
                    case T_RANGLE:  // Input
                        if(current.kind == T_VAR){
                            int idx = is_decl(current.ident);
                            if(idx == -1){
                                if(decl.count < 26){
                                    decl.variables[decl.count] = (ref_t){current.ident, 0};
                                    idx = decl.count;
                                    decl.count++;
                                }
                                else idx = -1;
                            }
                            if(idx != -1){
                                printf("%c: ", decl.variables[idx].ident);
                                scanf("%d", &decl.variables[idx].value);
                            }
                        }
                        break;
                    case T_LANGLE:  // Output
                        if(current.kind == T_VAR){
                            int idx = is_decl(current.ident);
                            if(idx != -1) printf("%c = %d\n", decl.variables[idx].ident, decl.variables[idx].value);
                        }
                        else if(current.kind == T_NUM){
                            printf("%d\n", current.number);
                        }
                        else if(current.kind == T_EOL || current.kind == T_EOF) continue;
                        else fprintf(stderr, "Unexpected token: %d\n", current.kind);
                        break;
                    default: break;
                }
                continue;
            case T_EOL: case T_EOF: continue;
            default:
                fprintf(stderr, "Unexpected token: %d\n", current.kind);
                continue;
        }
    }
}

void execute(char* code)
{
    tokenize(code);
    parse();
    free(tokens);
}
