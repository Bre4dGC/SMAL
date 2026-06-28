#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "smalc.h"

token_t* tokens;
int iter;

decl_t decl;
stmt_t stmt;

token_t current;
token_t next;

void tokenize(char* code)
{
    tokens = calloc(256, sizeof(token_t));
    int count = 0;
    bool line_start = true;

    for(int i = 0; code[i] != '\0'; i++){
        if(line_start){
            if(code[i] == '\t'){
                tokens[count].kind = T_TAB;
                count++;
                if(count >= 255) break;
                line_start = true;
                continue;
            }
            if(code[i] == ' '){
                int spaces = 0;
                while(code[i] == ' '){
                    spaces++;
                    i++;
                }
                if(spaces >= 4){
                    tokens[count].kind = T_TAB;
                    count++;
                    if(count >= 255) break;
                }
                i--;
                continue;
            }
        }

        switch(code[i]){
            case '*': case '/': case '+': case '-': case '=': case '<': case '>': case '?': case '@':
                tokens[count].kind = code[i]; line_start = false;
                break;
            case '\n': tokens[count].kind = T_EOL; line_start = true; break;
            case ' ': continue;
            case '#':
                while(code[i] != '\n' && code[i] != '\0') i++;
                i--;
                continue;
            case 'a'...'z':
                tokens[count].kind = T_VAR;
                tokens[count].ident = code[i];
                line_start = false;
                break;
            default:
                if(isdigit(code[i])){
                    tokens[count].kind = T_NUM;
                    tokens[count].number = 0;
                    line_start = false;

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

void advance()
{
    iter++;
    current = tokens[iter];
    next = tokens[iter+1];
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

int parse_expr()
{
    int value = 0;
    int left = get_token_value(&current);

    if(next.kind == T_EOL || next.kind == T_EOF){
        advance();
        return left;
    }

    token_t *right_tok = &tokens[iter + 2];
    int right = get_token_value(right_tok);

    switch(next.kind){
        case T_MUL: value = left * right; break;
        case T_DIV: value = left / right; break;
        case T_ADD: value = left + right; break;
        case T_SUB: value = left - right; break;
        default: value = left; break;
    }
    advance();
    advance();
    return value;
}

void parse_stmt(void)
{
    int stmt_start = iter - 2;
    int stmt_line_end = iter + 2;

    while(tokens[stmt_line_end].kind != T_EOL && tokens[stmt_line_end].kind != T_EOF) stmt_line_end++;

    int body_start = -1;
    if(tokens[stmt_line_end].kind == T_EOL && tokens[stmt_line_end + 1].kind == T_TAB){
        body_start = stmt_line_end + 2;
    }

    int body_end = stmt_line_end;
    if(body_start != -1){
        body_end = body_start;
        while(tokens[body_end].kind != T_EOF){
            if(tokens[body_end].kind == T_EOL && tokens[body_end + 1].kind != T_TAB) break;
            body_end++;
        }
    }

    int after_block = body_end + 1;

    if(!stmt.cond){
        stmt.loop_back = -1;
        stmt.jump_to = -1;
        iter = after_block - 1;
        return;
    }
    stmt.loop_back = stmt_start;
    stmt.jump_to = after_block;
    iter = stmt_line_end;
}

void parse(void)
{
    ref_t* ref = NULL;

    for(iter = 0; tokens[iter].kind != T_EOF; iter++){
        if(stmt.jump_to != -1 && iter == stmt.jump_to){
            iter = stmt.loop_back - 1;
            continue;
        }

        current = tokens[iter];
        next = tokens[iter+1];

        switch(current.kind){
            case T_VAR:
                switch(next.kind){
                    case T_VAR: case T_EOL: case T_EOF:
                        while(current.kind == T_VAR){
                            if(is_decl(current.ident) == -1){
                                if(decl.count < 26){
                                    decl.variables[decl.count] = (ref_t){current.ident, 0};
                                    decl.count++;
                                }
                            }
                            advance();
                        }
                        break;
                    case T_ASSIGN: {
                        int idx = is_decl(current.ident);
                        ref = &decl.variables[idx];
                        advance();
                        advance();
                        ref->value = parse_expr();
                    } break;
                    case T_MUL: case T_DIV: case T_ADD: case T_SUB: {
                        int idx = is_decl(current.ident);
                        if(idx == -1) return;
                        decl.variables[idx].value = parse_expr();
                        advance();
                    } break;
                    case T_LANGLE: case T_RANGLE: {
                        int stmt_start = iter;
                        stmt.left = get_token_value(&current);
                        token_kind_t rel = next.kind;
                        token_t *right_tok = &tokens[iter + 2];
                        token_kind_t stmt_token = tokens[iter + 3].kind;
                        stmt.right = get_token_value(right_tok);

                        if(stmt_token == T_COND || stmt_token == T_LOOP){
                            stmt.type = (stmt_token == T_COND ? STMT_COND : STMT_LOOP);
                            iter += 2;
                            current = tokens[iter];
                            next = tokens[iter+1];

                            stmt.cond = (rel == T_LANGLE && stmt.left < stmt.right) || (rel == T_RANGLE && stmt.left > stmt.right);
                            if(stmt.cond){
                                parse_stmt();
                                if(stmt.jump_to != -1 && iter == stmt.jump_to) iter = stmt.loop_back - 1;
                                break;
                            }
                            parse_stmt();
                            break;
                        }
                        fprintf(stderr, "Expected '?' or '@' instead of: %d\n", stmt_token);
                    } break;
                    default:
                        fprintf(stderr, "Unexpected token: %d\n", current.kind);
                        continue;
                } break;
            case T_LANGLE: case T_RANGLE: {
                token_kind_t stream = current.kind;
                advance();
                switch(stream){
                    // Input
                    case T_RANGLE:
                        if(current.kind == T_VAR){
                            int idx = is_decl(current.ident);
                            if(idx != -1){
                                printf("%c: ", decl.variables[idx].ident);
                                scanf("%d", &decl.variables[idx].value);
                            }
                        }
                        break;
                    // Output
                    case T_LANGLE:
                        if(current.kind == T_VAR){
                            int idx = is_decl(current.ident);
                            if(idx != -1) printf("%c = %d\n", decl.variables[idx].ident, parse_expr());
                        }
                        else if(current.kind == T_NUM) printf("%d\n", parse_expr());
                        else if(current.kind == T_EOL || current.kind == T_EOF) continue;
                        else fprintf(stderr, "Unexpected token: %d\n", current.kind);
                        break;
                    default: break;
                }
            } continue;
            case T_EOL: case T_EOF: case T_TAB: continue;
            default:
                fprintf(stderr, "Unexpected token: %d\n", current.kind);
                continue;
        }
    }
}

void execute(char* filename)
{
    FILE* file = fopen(filename, "r");
    if(file == NULL) fprintf(stderr, "Could not open file: %s\n", filename);

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if(buffer == NULL) fprintf(stderr, "Memory allocation failed\n");

    if(fread(buffer, 1, length, file) != length) fprintf(stderr, "Could not read file: %s\n", filename);
    buffer[length] = '\0';

    decl.count = 0;
    iter = 0;
    stmt.jump_to = -1;
    stmt.loop_back = -1;

    tokenize(buffer);
    parse();

    free(tokens);
    free(buffer);
    fclose(file);
}

void error(char* msg)
{
    fprintf(stderr, "%s", msg);
    exit(1);
}
