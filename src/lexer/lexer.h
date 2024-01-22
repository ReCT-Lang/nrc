#pragma once
#include <stdio.h>
#include "token.h"

typedef struct {
    token_t* tokens;
    int token_count;
    int tokens_allocated;
    location loc;
    char* data;
    unsigned long position;
    unsigned long data_length;
} lexer_context;

lexer_context* lexer_create();
void lexer_push(lexer_context* context, char* data, int length);
void lexer_read(lexer_context* context, FILE* file);
void lexer_destroy(lexer_context* context);
void lexer_process(lexer_context* context);