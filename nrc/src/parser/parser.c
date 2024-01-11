#include <string.h>
#include "parser.h"
#include "stdlib.h"

#define CONSTRUCT(_context, _type, _id) ({ \
        _type* data = (_type*)palloc(_context, sizeof(_type)); \
        memset(data, 0, sizeof(_type)); \
        data->type = _id;                                   \
        data;\
    })
#define NEW_LIST(_context) ({ \
        node_list* list = (node_list*)palloc(_context, sizeof(node_list)); \
        list->length = 0;     \
        list->allocated = 0;  \
        list->data = NULL;    \
        list;\
    })

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

static void list_push(parser_context* context, node_list* list, node* data) {
    // If we can't fit more data into the list, we need to allocate more memory.
    if(list->allocated < list->length + 1) {
        if(list->data == NULL) {
            // A new list won't have any data
            list->data = palloc(context, 1);
            list->allocated = 1;
        } else {
            node** new_data = msalloc(context->allocation_stack, (int)sizeof(node*) * list->allocated * 2);
            memccpy(new_data, list->data, sizeof(node*), list->allocated);
            msdealloc(context->allocation_stack, list->data);
            list->allocated = list->allocated * 2;
            list->data = new_data;
        }
    }
    list->data[list->length] = data;
    list->length++;
}

static package_def_node* parse_package(parser_context* parser) {
    consume(parser, TOKEN_KW_PACKAGE);

    token_t name = consume(parser, TOKEN_ID);
    package_def_node* root_node = CONSTRUCT(parser, package_def_node, NODE_PACKAGE_DEF);

    root_node->package_name = palloc(parser, (int)strlen(name.data));
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
    node_root* root_node = CONSTRUCT(parser, node_root, NODE_ROOT);
    root_node->children = NEW_LIST(parser);
    parser->node = (node*)root_node;

    while(current(parser).type != TOKEN_EOF) {
        list_push(parser, root_node->children, parse_anything(parser));
    }
}