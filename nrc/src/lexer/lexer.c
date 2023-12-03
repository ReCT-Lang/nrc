#include "lexer.h"
#include <stdlib.h>
#include "token.h"

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

void lexer_process(lexer_context* context, FILE* file) {
    int c;
    while((c = fgetc(file)) != EOF) {
        switch (c) {
            // Escape characters
            case ' ':
                break;
            case '\n':
                break;
            case '\t':
                break;
            case '\r':
                break;

            case ';':
                push_empty_token(context, TOKEN_END_STMT);
                break;

            default:
                fprintf(stderr, "Invalid token %2X(%c)\n", c, c);
        }
    }
    push_empty_token(context, TOKEN_EOF);
}