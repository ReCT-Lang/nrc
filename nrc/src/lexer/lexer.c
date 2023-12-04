#include "lexer.h"
#include <stdlib.h>
#include "token.h"
#include <string.h>

static void push_token(lexer_context* lexer, token_t token) {
    if(lexer->tokens_allocated <= lexer->token_count + 1) {
        lexer->tokens = reallocarray(lexer->tokens, lexer->tokens_allocated * 2 + 1, sizeof(lexer_context));
        lexer->tokens_allocated = lexer->tokens_allocated * 2 + 1;
    }
    lexer->tokens[lexer->token_count] = token;
    lexer->token_count++;
}

static void push_empty_token(lexer_context* lexer, token_type_e type) {
    token_t token;
    token.type = type;
    token.data = NULL;
    push_token(lexer, token);
}

lexer_context* lexer_create() {
    lexer_context* lexer = (lexer_context*)malloc(sizeof(lexer_context));
    return lexer;
}

void lexer_destroy(lexer_context* context) {
    for (int i = 0; i < context->token_count; ++i) {
        free(context->tokens[i].data);
    }

    free(context);
}

static int read_cmp(FILE* file, const char* target) {
    int len = strlen(target);
    char* r_buf = (char*)malloc(len + 1);

    for (int i = 0; i < len; ++i) {
        r_buf[i] = fgetc(file);
    }
    for (int i = len - 1; i >= 0; --i) {
        ungetc(r_buf[i], file);
    }
    r_buf[len] = 0;

    int eq = strcmp(r_buf, target);
    free(r_buf);
    return eq == 0 ? 1 : 0;
}

#define CASE_SKIP(_char) if(c == _char) continue;
#define CASE_SIMPLE(_char, _type) if(c == _char) { push_empty_token(context, _type); continue; }
#define CASE_SIMPLE_MULT(_str, _type) if(read_cmp(file, _str)) { push_empty_token(context, _type); continue; }

void lexer_process(lexer_context* context, FILE* file) {
    int c;
    while((c = fgetc(file)) != EOF) {

        CASE_SKIP(' ')
        CASE_SKIP('\t')
        CASE_SKIP('\n')
        CASE_SKIP('\r')
        CASE_SIMPLE(';', TOKEN_END_STMT)

        fprintf(stderr, "Invalid token %2X(%c)\n", c, c);
    }
    push_empty_token(context, TOKEN_EOF);
}