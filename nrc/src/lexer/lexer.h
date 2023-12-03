#pragma once
#include <stdio.h>
#include "token.h"

typedef struct {
    token_t* tokens;
    int token_count;
    int tokens_allocated;

} lexer_context;

lexer_context* lexer_create();
void lexer_destroy(lexer_context* context);
void lexer_process(lexer_context* context, FILE* file);