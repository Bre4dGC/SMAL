#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "smalc.h"

#define TOKENS_LIMIT 256
#define STMT_LIMIT   3

token_t tokens[TOKENS_LIMIT];
int iter;

decl_t decl;
stmt_t stmt[STMT_LIMIT];
int stmt_depth;

token_t current;
token_t next;

void tokenize(char* code)
{
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
                int groups = spaces / 4;
                for(int g = 0; g < groups; g++){
                    tokens[count].kind = T_TAB;
                    count++;
                    if(count >= 255) break;
                }
                if(count >= 255) break;
                i--;
                continue;
            }
        }

        switch(code[i]){
            case '*': case '/': case '+': case '-':
            case '=': case '~': case '?': case '@':
            case '<': case '>': case ':': case '!':
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

                    size_t num = 0;
                    for(size_t j = 0; j < strlen(str); j++) num = num * 10 + (str[j] - '0');
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

static size_t get_token_value(token_t* t)
{
    if(t->kind == T_NUM) return t->number;
    if(t->kind == T_VAR){
        int idx = is_decl(t->ident);
        if(idx != -1) return decl.variables[idx].value;
        return 0;
    }
    return 0;
}

size_t parse_expr()
{
    size_t left = get_token_value(&current);

    if(next.kind == T_EOL || next.kind == T_EOF){
        advance();
        return left;
    }

    token_t *right_tok = &tokens[iter + 2];
    size_t right = get_token_value(right_tok);

    switch(next.kind){
        case T_MUL: left *= right; break;
        case T_DIV: left /= right; break;
        case T_ADD: left += right; break;
        case T_SUB: left -= right; break;
        case T_RAND:
            left += rand() % (right + 1);
            break;
        default: break;
    }
    advance();
    advance();
    return left;
}

void parse_stmt(void)
{
    stmt_t* cs = &stmt[stmt_depth];
    int stmt_start = iter - 2;
    int stmt_line_end = iter + 2;

    while(tokens[stmt_line_end].kind != T_EOL && tokens[stmt_line_end].kind != T_EOF) stmt_line_end++;

    int body_start = -1;
    int body_tabs = 0;
    if(tokens[stmt_line_end].kind == T_EOL && tokens[stmt_line_end + 1].kind == T_TAB){
        body_start = stmt_line_end + 1;
        while(tokens[body_start].kind == T_TAB){
            body_tabs++;
            body_start++;
        }
    }

    int body_end = stmt_line_end;
    if(body_start != -1){
        body_end = body_start;
        while(tokens[body_end].kind != T_EOF){
            if(tokens[body_end].kind == T_EOL){
                int tabs = 0;
                while(tokens[body_end + 1 + tabs].kind == T_TAB) tabs++;
                if(tabs < body_tabs) break;
            }
            body_end++;
        }
        if(tokens[body_end].kind == T_EOF && body_end > body_start) body_end--;
    }

    int after_block = body_end + 1;

    if(!cs->cond){
        cs->loop_back = -1;
        cs->jump_to = -1;
        iter = after_block - 1;
        return;
    }
    cs->loop_back = stmt_start;
    cs->jump_to = after_block;
    
    if(body_start != -1) iter = body_start - 1;
    else iter = stmt_line_end;
}

void parse(void)
{
    ref_t* ref = NULL;

    for(iter = 0; ; iter++){
        for (int d = stmt_depth - 1; d >= 0; d--) {
            if (stmt[d].jump_to != -1 && stmt[d].type == STMT_COND && iter >= stmt[d].jump_to) {
                stmt[d].jump_to = -1;
                stmt_depth = d;
            }
        }
        for (int d = stmt_depth - 1; d >= 0; d--) {
            if (stmt[d].jump_to != -1 && stmt[d].type == STMT_LOOP && iter >= stmt[d].jump_to) {
                iter = stmt[d].loop_back - 1;
                stmt_depth = d;
                continue;
            }
        }
        if(iter < 0) continue;
        if(tokens[iter].kind == T_EOF) break;

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
                        size_t idx = is_decl(current.ident);
                        ref = &decl.variables[idx];
                        advance();
                        advance();
                        ref->value = parse_expr();
                    } break;
                    case T_MUL: case T_DIV: case T_ADD: case T_SUB: case T_RAND: {
                        int idx = is_decl(current.ident);
                        if(idx == -1) return;
                        decl.variables[idx].value = parse_expr();
                        advance();
                    } break;
                    case T_LANGLE: case T_RANGLE: case T_EQUAL: case T_NEQUAL: {
                        stmt_t* cs = &stmt[stmt_depth];
                        cs->left = get_token_value(&current);
                        token_kind_t rel = next.kind;
                        token_t *right_tok = &tokens[iter + 2];
                        token_kind_t stmt_token = tokens[iter + 3].kind;
                        cs->right = get_token_value(right_tok);

                        if(stmt_token == T_COND || stmt_token == T_LOOP){
                            cs->type = stmt_token;
                            iter++;
                            advance();

                            cs->cond = (rel == T_LANGLE && cs->left < cs->right)
                                    || (rel == T_RANGLE && cs->left > cs->right)
                                    || (rel == T_EQUAL  && cs->left == cs->right)
                                    || (rel == T_NEQUAL && cs->left != cs->right);
                            if(cs->cond){
                                parse_stmt();
                                stmt_depth++;
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
                                scanf("%zd", &decl.variables[idx].value);
                            }
                        }
                        break;
                    // Output
                    case T_LANGLE:
                        if(current.kind == T_VAR){
                            int idx = is_decl(current.ident);
                            if(idx != -1) printf("%c = %zd\n", decl.variables[idx].ident, parse_expr());
                        }
                        else if(current.kind == T_NUM) printf("%zd\n", parse_expr());
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
    stmt_depth = 0;
    for (int i = 0; i < STMT_LIMIT; i++) {
        stmt[i].jump_to = -1;
        stmt[i].loop_back = -1;
    }

    srand(time(NULL));
    tokenize(buffer);
    parse();

    free(buffer);
    fclose(file);
}

void repl(void)
{
    srand(time(NULL));
    char source[1028 * 10];
    char line[1028];
    while(true){
        printf("smal> ");
        if(!fgets(line, sizeof(line), stdin)) break;
        if(strcmp(line, "Q\n") == 0) break;
        else if(strcmp(line, "C\n") == 0){
            decl.count = 0;
            iter = 0;
            stmt_depth = 0;
            for(int i = 0; i < STMT_LIMIT; i++){
                stmt[i].jump_to = -1;
                stmt[i].loop_back = -1;
            }
            continue;
        }

        source[0] = '\0';
        strcat(source, line);

        size_t len = strlen(source);
        size_t end = len;
        while(end > 0 && (source[end-1] == '\n')) end--;
        bool has_stmt = end > 0 && (source[end-1] == '?' || source[end-1] == '@');

        if(has_stmt){
            while(true){
                printf("smal> ... ");
                if(!fgets(line, sizeof(line), stdin)) break;
                if(strcmp(line, "\n") == 0) break;
                if(strlen(source) + strlen(line) < sizeof(source)){
                    source[strlen(source)] = '\t';
                    strcat(source, line);
                }
            }
        }

        stmt_depth = 0;
        for(int i = 0; i < STMT_LIMIT; i++){
            stmt[i].jump_to = -1;
            stmt[i].loop_back = -1;
        }

        tokenize(source);
        parse();
    }
}

void error(char* msg)
{
    fprintf(stderr, "[ERROR] %s", msg);
    exit(1);
}
