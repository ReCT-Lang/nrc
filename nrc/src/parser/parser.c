#include <string.h>
#include "parser.h"
#include "stdlib.h"
#include "nodes.h"
#include "../errors/error.h"

#define CAST(_to, _val) ((_to)_val)

static node_function_body* parse_function_body(parser_context* parser);
static node* parse_statement(parser_context* parser);
static node* parse_class_body(parser_context* parser);
static node_variable_def* parse_variable_definition(parser_context* parser, permissions perms);

static void throw_invalid_token(token_t token) {
    if(token.data == NULL)
        error_throw("RCT2001", token.loc, "Unexpected token %s", TOKEN_NAMES[token.type]);
    else
        error_throw("RCT2001", token.loc, "Unexpected token %s (%s)", token.data, TOKEN_NAMES[token.type]);
}

void* palloc(parser_context* context, int size) {
    return msalloc(context->allocation_stack, size);
}

static string copy_string(parser_context* context, string src) {
    string str = (string)palloc(context, (int)strlen(src) + 1);
    strcpy(str, src);
    return str;
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

static int at(parser_context* parser, token_type_e type) {
    return current(parser).type == type;
}

static void step(parser_context* parser, int amt) {
    parser->token_current += amt;
}

static token_t consume(parser_context* parser, token_type_e type) {
    token_t c = current(parser);
    step(parser, 1);
    if(c.type != type) {
        error_throw("RCT2002", current(parser).loc, "Unexpected token %s, expected %s",
                    TOKEN_NAMES[c.type], TOKEN_NAMES[type]);
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

static node_identifier* parse_identifier(parser_context* parser, int level) {

    int reference = 0;
    if(at(parser, TOKEN_KW_REF)) {
        if(level != 0) {
            error_throw("RCT2011", current(parser).loc, "Reference declarations may only be top-level");
        }
        reference = 1;
        consume(parser, TOKEN_KW_REF);
    }

    token_t id = consume(parser, TOKEN_ID);

    node_identifier* identifier = new_node_identifier(parser);
    identifier->pointer = reference;
    identifier->name = copy_string(parser, id.data);

    if(at(parser, TOKEN_PACKAGE_ACCESS)) {
        if(level > 0)
            error_throw("RCT2010", current(parser).loc, "Package access may not be nested!");
        consume(parser, TOKEN_PACKAGE_ACCESS);
        identifier->child = parse_identifier(parser, level + 1);
        identifier->package = 1;
    } else if(at(parser, TOKEN_ACCESS)) {
        consume(parser, TOKEN_ACCESS);
        identifier->child = parse_identifier(parser, level + 1);
    }

    return identifier;
}

static node_package_def* parse_package(parser_context* parser) {
    consume(parser, TOKEN_KW_PACKAGE);

    token_t name = consume(parser, TOKEN_ID);
    node_package_def* root_node = new_node_package_def(parser);

    root_node->package_name = copy_string(parser, name.data);

    consume(parser, TOKEN_END_STMT);

    return root_node;
}

static node_class_def* parse_class(parser_context* parser, permissions perms) {
    consume(parser, TOKEN_KW_CLASS);
    node_class_def* class = new_node_class_def(parser);

    token_t name = consume(parser, TOKEN_ID);
    class->name = copy_string(parser, name.data);
    class->flags = perms;

    consume(parser, TOKEN_BRACE_OPEN);

    while (!at(parser, TOKEN_BRACE_CLOSE)) {
        node* child = parse_class_body(parser);
        if(child != NULL)
            list_push(parser, class->children, child);
    }
    consume(parser, TOKEN_BRACE_CLOSE);


    return class;
}

static node_function_def* parse_function(parser_context* parser, permissions perms) {
    consume(parser, TOKEN_KW_FUNCTION);
    node_function_def* function = new_node_function_def(parser);
    function->flags = perms;

    token_t function_name = consume(parser, TOKEN_ID);
    function->name = copy_string(parser, function_name.data);


    consume(parser, TOKEN_PARENTHESIS_OPEN);
    while (1) {
        if(current(parser).type == TOKEN_PARENTHESIS_CLOSE)
            break;

        token_t name = consume(parser, TOKEN_ID);
        node_identifier* type = parse_identifier(parser, 0);

        // TODO: Push these to a list

        if(current(parser).type == TOKEN_PARENTHESIS_CLOSE)
            break;
        consume(parser, TOKEN_COMMA);
    }
    consume(parser, TOKEN_PARENTHESIS_CLOSE);

    if(current(parser).type == TOKEN_ACCESS) {
        consume(parser, TOKEN_ACCESS);
        // For now, we don't support custom types, just raw IDs.
        node_identifier* returnType = parse_identifier(parser, 0);
        function->return_type = returnType;
    }

    // Extern functions may not have a body.
    if(perms & PERMS_EXTERN) {
        consume(parser, TOKEN_END_STMT);
        return function;
    }

    // TODO: Body parsing(in another function)

    function->body = parse_function_body(parser);

    return function;
}

static node* parse_definition(parser_context* parser) {
    permissions perms = PERMS_NONE;

    while (1) {
        if(current(parser).type == TOKEN_KW_PRIVATE) {
            perms |= PERMS_PRIVATE;
            consume(parser, TOKEN_KW_PRIVATE);
        } else if(current(parser).type == TOKEN_KW_SET) {
            perms |= PERMS_PUBLIC;
            consume(parser, TOKEN_KW_SET);
        } else if(current(parser).type == TOKEN_KW_STATIC) {
            perms |= PERMS_STATIC;
            consume(parser, TOKEN_KW_STATIC);
        } else if(current(parser).type == TOKEN_KW_UNSAFE) {
            perms |= PERMS_UNSAFE;
            consume(parser, TOKEN_KW_UNSAFE);
        } else if(current(parser).type == TOKEN_KW_EXTERN) {
            perms |= PERMS_EXTERN;
            consume(parser, TOKEN_KW_EXTERN);
        } else if(current(parser).type == TOKEN_KW_FUNCTION) {
            return (node*)parse_function(parser, perms);
        } else if(current(parser).type == TOKEN_KW_CLASS) {
            return (node*)parse_class(parser, perms);
        } else if(current(parser).type == TOKEN_KW_VAR) {
            return (node*) parse_variable_definition(parser, perms);
        } else {
            throw_invalid_token(current(parser));
            break;
        }
    }

    return NULL;
}

static node_function_body* parse_function_body(parser_context* parser) {
    consume(parser, TOKEN_BRACE_OPEN);

    node_function_body* body = new_node_function_body(parser);

    while (!at(parser, TOKEN_BRACE_CLOSE)) {
        node* stmt = parse_statement(parser);
        if(stmt != NULL)
            list_push(parser, body->children, stmt);
    }
    consume(parser, TOKEN_BRACE_CLOSE);

    return body;
}

static node* parse_statement(parser_context* parser) {
    token_t c = current(parser);

    if(c.type == TOKEN_KW_VAR) return (node*)parse_variable_definition(parser, PERMS_NONE);

    step(parser, 1);
    return NULL;
}

static node_variable_def* parse_variable_definition(parser_context* parser, permissions perms) {

    // [permissions*] [name*] -> [type] <- [value]
    // * - mandatory
    // We either need a type or a value, but we'll leave that for
    // later determination.

    consume(parser, TOKEN_KW_VAR);

    node_variable_def* def = new_node_variable_def(parser);
    def->flags = perms;
    def->name = copy_string(parser, consume(parser, TOKEN_ID).data);

    // We've got a type.
    if(at(parser, TOKEN_ACCESS)) {
        consume(parser, TOKEN_ACCESS);
        def->value_type = parse_identifier(parser, 0);
    }

    if(at(parser, TOKEN_ASSIGN)) {
        consume(parser, TOKEN_ASSIGN);
        // TODO: We need to parse some sort of statement.
    }

    consume(parser, TOKEN_END_STMT);

    return def;
}

// Top-level statements, so, package includes, class, enum, function, struct & global defines
static node* parse_top(parser_context* parser) {
    token_t c = current(parser);

    if(c.type == TOKEN_KW_PACKAGE) return (node*) parse_package(parser);

    if(c.type == TOKEN_KW_SET) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_PRIVATE) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_STATIC) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_EXTERN) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_UNSAFE) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_VAR) return (node*) parse_variable_definition(parser, PERMS_PRIVATE);
    if(c.type == TOKEN_KW_FUNCTION) return (node*) parse_function(parser, PERMS_PRIVATE);
    if(c.type == TOKEN_KW_CLASS) return (node*) parse_class(parser, PERMS_PRIVATE);

    throw_invalid_token(c);
    step(parser, 1);
    return NULL;
}

static node* parse_class_body(parser_context* parser) {
    token_t c = current(parser);

    if(c.type == TOKEN_KW_SET) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_PRIVATE) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_STATIC) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_EXTERN) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_UNSAFE) return (node*) parse_definition(parser);
    if(c.type == TOKEN_KW_VAR) return (node*) parse_variable_definition(parser, PERMS_PRIVATE);
    if(c.type == TOKEN_KW_FUNCTION) return (node*) parse_function(parser, PERMS_PRIVATE);
    if(c.type == TOKEN_KW_CLASS) return (node*) parse_class(parser, PERMS_PRIVATE);

    throw_invalid_token(c);
    step(parser, 1);
    return NULL;
}

void parser_parse(parser_context* parser) {
    node_root* root_node = new_node_root(parser);
    parser->node = (node*)root_node;

    while(current(parser).type != TOKEN_EOF) {
        node* n = parse_top(parser);
        if(n != NULL)
            list_push(parser, root_node->children, n);
    }
}