#pragma once

#include <memstack.h>
#include "node.h"
#include "../lexer/lexer.h"

typedef struct {
    memstack* allocation_stack;
    node node;
} parser_context;

parser_context* parser_create();
void parser_destroy(parser_context* parser);

void parser_parse(parser_context* parser, lexer_context* lexer);