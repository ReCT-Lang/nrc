#include "parser.h"
#include "stdlib.h"

void* palloc(parser_context* context, int size) {
    return msalloc(context->allocation_stack, size);
}

parser_context* parser_create() {
    parser_context* context = (parser_context*)malloc(sizeof(parser_context));
    context->allocation_stack = msnew();

    return context;
}

void parser_destroy(parser_context* parser) {
    msfree(parser->allocation_stack);
    free(parser);
}

#define CAST(_to, _val) ((_to)_val)

void parser_parse(parser_context* parser, lexer_context* lexer) {
    parser->node = palloc(parser, sizeof(node));
    CAST(node_root, parser->node)->type = NODE_ROOT;

    // TODO: Parser :)
}