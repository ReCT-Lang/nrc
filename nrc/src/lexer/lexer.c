#include "lexer.h"
#include <stdlib.h>

static void push_token(lexer_context* lexer, token_t token) {
    if(lexer->tokens_allocated <= lexer->token_count + 1) {
        reallocarray(lexer->tokens, lexer->tokens_allocated, sizeof(lexer_context));
    }
}

lexer_context* lexer_create() {
    lexer_context* lexer = (lexer_context*)malloc(sizeof(lexer_context));
    return lexer;
}

void lexer_destroy(lexer_context* context) {
    for (int i = 0; i < context->token_count; ++i) {
        free(context->tokens[i].value);
    }

    free(context);
}

void lexer_process(lexer_context* context, FILE* file) {

}