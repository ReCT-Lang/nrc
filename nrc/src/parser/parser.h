#pragma once

#include <memstack.h>
#include "node.h"
#include "../lexer/lexer.h"

typedef struct {
    memstack* allocation_stack;
    node* node;
    lexer_context* lexer;
    int token_current;
} parser_context;

parser_context* parser_create(lexer_context* lexer);
void parser_destroy(parser_context* parser);

void parser_parse(parser_context* parser);