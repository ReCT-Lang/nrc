#include <string.h>
#include "parser.h"
#include "stdlib.h"
#include "nodes.h"


#define CAST(_to, _val) ((_to)_val)

void* palloc(parser_context* context, int size) {
    return msalloc(context->allocation_stack, size);
}

static token_t peek(parser_context* parser, int amt) {
    int index = parser->token_current + amt;
    if(index >= parser->lexer->token_count) {
        return (token_t) { .data = NULL, .type = TOKEN_EOF };
    }
    return parser->lexer->tokens[index];
}

static token_t current(parser_context* parser) {
    return peek(parser, 0);
}

static void step(parser_context* parser, int amt) {
    parser->token_current += amt;
}

static token_t consume(parser_context* parser, token_type_e type) {
    token_t c = current(parser);
    step(parser, 1);
    if(c.type != type) {
        // TODO: Proper exception throwing
        fprintf(stderr, "Expected token %s, got %s\n", TOKEN_NAMES[type], TOKEN_NAMES[c.type]);
        exit(1);
    }
    return c;
}


parser_context* parser_create(lexer_context* lexer) {
    parser_context* context = (parser_context*)malloc(sizeof(parser_context));
    context->allocation_stack = msnew();
    context->lexer = lexer;
    context->token_current = 0;

    return context;
}

void parser_destroy(parser_context* parser) {
    msfree(parser->allocation_stack);
    free(parser);
}

static node_package_def* parse_package(parser_context* parser) {
    consume(parser, TOKEN_KW_PACKAGE);

    token_t name = consume(parser, TOKEN_ID);
    node_package_def* root_node = new_node_package_def(parser);

    root_node->package_name = palloc(parser, (int)strlen(name.data) + 1);
    strcpy(root_node->package_name, name.data);

    consume(parser, TOKEN_END_STMT);

    return root_node;
}

static node* parse_anything(parser_context* parser) {
    token_t c = current(parser);

    if(c.type == TOKEN_KW_PACKAGE) return (node*) parse_package(parser);

    fprintf(stderr, "Unexpected token %s\n", TOKEN_NAMES[c.type]);
    step(parser, 1);
    return NULL;
}

void parser_parse(parser_context* parser) {
    node_root* root_node = new_node_root(parser);
    parser->node = (node*)root_node;

    while(current(parser).type != TOKEN_EOF) {
        list_push(parser, root_node->children, parse_anything(parser));
    }
}