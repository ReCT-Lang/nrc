#include <string.h>
#include "parser.h"
#include "stdlib.h"
#include "nodes.h"
#include "../errors/error.h"

static node* parse_statement(parser_context* parser, int semicolon);

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

node_literal* parse_literal(parser_context* parser) {
    node_literal* literal = new_node_literal(parser);
    literal->value = copy_string(parser, current(parser).data);
    step(parser, 1);
    return literal;
}

node_identifier* parse_identifier(parser_context* parser)  {
    node_identifier* identifier = new_node_identifier(parser);

    token_t name_token = consume(parser, TOKEN_ID);
    identifier->name = copy_string(parser, name_token.data);

    if(at(parser, TOKEN_PACKAGE_ACCESS)) {
        consume(parser, TOKEN_PACKAGE_ACCESS);
        // If we're at a package access we can't have any generics etc
        identifier->package = 1;
        identifier->child = parse_identifier(parser);

        return identifier;
    }

    // If it's not a package access, we first handle generics, then child identifiers.
    if(at(parser, TOKEN_BRACE_OPEN)) {
        consume(parser, TOKEN_BRACE_OPEN);
        while (1) {
            // We get the generic value
            node* generic = parse_statement(parser, 0);
            if(generic != NULL) {
                list_push(parser, identifier->generic_values, generic);
            }

            // If the next one isn't a comma it means we're done here.
            if(!at(parser, TOKEN_COMMA))
                break;

            consume(parser, TOKEN_COMMA);
        }
        // And then end it with the closing brace.
        consume(parser, TOKEN_BRACE_CLOSE);
    }

    // Now let's get our child if it exists.
    if(at(parser, TOKEN_ACCESS)) {
        consume(parser, TOKEN_ACCESS);
        identifier->child = parse_identifier(parser);
    }

    return identifier;
}

node_package_def* parse_package(parser_context* parser) {
    consume(parser, TOKEN_KW_PACKAGE);
    node_package_def* package = new_node_package_def(parser);

    token_t name_token = consume(parser, TOKEN_ID);
    package->package_name = copy_string(parser, name_token.data);

    return package;
}

node_use_def* parse_package_use(parser_context* parser) {
    consume(parser, TOKEN_KW_USE);
    node_use_def* package = new_node_use_def(parser);

    token_t name_token = consume(parser, TOKEN_ID);
    package->package_name = copy_string(parser, name_token.data);

    return package;
}

node* parse_variable_definition(parser_context* parser, permissions perms) {
    node_variable_def* variable_def = new_node_variable_def(parser);
    variable_def->flags = perms;

    // The structure is:
    // [modifiers] var [name] (-> [type]) (<- [statement]);
    // Modifiers is handled in the preceding step.
    consume(parser, TOKEN_KW_VAR);
    variable_def->name = copy_string(parser, consume(parser, TOKEN_ID).data);

    if(at(parser, TOKEN_ACCESS)) {
        // We've got a type!
        consume(parser, TOKEN_ACCESS);

        variable_def->value_type = parse_identifier(parser);
    }

    if(at(parser, TOKEN_ASSIGN)) {
        // We've got a default type!
        consume(parser, TOKEN_ASSIGN);
        variable_def->default_value = parse_statement(parser, 0);
    }

    consume(parser, TOKEN_END_STMT);

    return (node*)variable_def;
}

node* parse_function_definition(parser_context* parser, permissions perms) {
    node_function_def* function_def = new_node_function_def(parser);
    function_def->flags = perms;

    // Syntax:
    // [modifiers] function [name]([parameters]) (-> [return type])
    // Then either { [body] } or a semicolon, depending on the function declaration.
    // Modifiers are already handled.
    // No return type means it's a void.
    consume(parser, TOKEN_KW_FUNCTION);

    // [name]
    function_def->name = copy_string(parser, consume(parser, TOKEN_ID).data);

    // Parameters
    consume(parser, TOKEN_PARENTHESIS_OPEN);
    if(!at(parser, TOKEN_PARENTHESIS_CLOSE)) { // If we're not immediately at a close there's values.
        while (1) {

            // A parameter is
            // [name] [type] (*<- [default value])
            // Followed by a comma if there's multiple.

            node_parameter* parameter = new_node_parameter(parser);
            parameter->name = copy_string(parser, consume(parser, TOKEN_ID).data);
            parameter->value_type = parse_identifier(parser);

            if(at(parser, TOKEN_ASSIGN)) {
                consume(parser, TOKEN_ASSIGN);
                parameter->default_value = parse_statement(parser, 0);
            }

            list_push(parser, function_def->parameters, (node*)parameter);

            if(!at(parser, TOKEN_COMMA))
                break;

            consume(parser, TOKEN_COMMA);
        }
    }
    consume(parser, TOKEN_PARENTHESIS_CLOSE);

    if(at(parser, TOKEN_ACCESS)) {
        consume(parser, TOKEN_ACCESS);
        function_def->return_type = parse_identifier(parser);
    }

    // Extern functions don't declare a body
    if(perms & PERMS_EXTERN) {
        consume(parser, TOKEN_END_STMT);
        return (node*)function_def;
    }

    // TODO: Function body parsing.

    return (node*)function_def;
}

node* parse_class_definition(parser_context* parser, permissions perms) {
    node_class_def* class_def = new_node_class_def(parser);
    class_def->flags = perms;

    // Structure:
    // [modifiers] class [name]{[generics]} { ... }

    consume(parser, TOKEN_KW_CLASS);

    class_def->name = copy_string(parser, consume(parser, TOKEN_ID).data);

    if(at(parser, TOKEN_GENERIC_OPEN)) {
        consume(parser, TOKEN_GENERIC_OPEN);

        while (1) {
            node_literal* literal = new_node_literal(parser);
            literal->value = copy_string(parser, consume(parser, TOKEN_ID).data);
            list_push(parser, class_def->generics, (node*)literal);

            if(!at(parser, TOKEN_COMMA))
                break;
            consume(parser, TOKEN_COMMA);
        }

        consume(parser, TOKEN_GENERIC_CLOSE);
    }

    // TODO: Class body

    return (node*)class_def;
}

node* parse_struct_definition(parser_context* parser, permissions perms) {
    consume(parser, TOKEN_KW_STRUCT);
    node_struct_def* struct_def = new_node_struct_def(parser);
    struct_def->flags = perms;

    return (node*)struct_def;
}

node* parse_definition(parser_context* parser) {
    permissions perms = PERMS_NONE;

    while (1) {
        // Basic permissions
        if(at(parser, TOKEN_KW_PRIVATE)) {
            consume(parser, TOKEN_KW_PRIVATE);
            perms |= PERMS_PRIVATE;
        } else if(at(parser, TOKEN_KW_SET)) {
            consume(parser, TOKEN_KW_SET);
            perms |= PERMS_PUBLIC;
        } else if(at(parser, TOKEN_KW_STATIC)) {
            consume(parser, TOKEN_KW_STATIC);
            perms |= PERMS_STATIC;
        } else if(at(parser, TOKEN_KW_EXTERN)) {
            consume(parser, TOKEN_KW_EXTERN);
            perms |= PERMS_EXTERN;
        } else if(at(parser, TOKEN_KW_UNSAFE)) {
            consume(parser, TOKEN_KW_UNSAFE);
            perms |= PERMS_UNSAFE;
        }

        // Definitions

        else if(at(parser, TOKEN_KW_VAR)) {
            return parse_variable_definition(parser, perms);
        } else if(at(parser, TOKEN_KW_FUNCTION)) {
            return parse_function_definition(parser, perms);
        } else if(at(parser, TOKEN_KW_CLASS)) {
            return parse_class_definition(parser, perms);
        } else if(at(parser, TOKEN_KW_STRUCT)) {
            return parse_struct_definition(parser, perms);
        }

        // Error
        else {
            error_throw("RCT2002", current(parser).loc, "Invalid token %s", TOKEN_NAMES[current(parser).type]);
            break;
        }
    }
}

// Return and optionally consume a semicolon.
// _semi is if this specific parse-ing should consume when needed.
// _value is the value to return.
#define PARSE(_semi, _value) { node* n = (node*)_value; if(semicolon && _semi) consume(parser, TOKEN_END_STMT); return n; }

// This is the main part of the parser.
// semicolons may be enforced when needed, but only for statements that need a semicolon.
node* parse_statement(parser_context* parser, int semicolon) {

    // Package-related statements
    if(at(parser, TOKEN_KW_PACKAGE)) PARSE(1, parse_package(parser))
    if(at(parser, TOKEN_KW_USE)) PARSE(1, parse_package_use(parser))

    // Definitions - They handle their own damn semicolons
    if(at(parser, TOKEN_KW_PRIVATE)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_SET)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_STATIC)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_EXTERN)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_UNSAFE)) PARSE(0, parse_definition(parser))

    if(at(parser, TOKEN_KW_VAR)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_FUNCTION)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_CLASS)) PARSE(0, parse_definition(parser))
    if(at(parser, TOKEN_KW_STRUCT)) PARSE(0, parse_definition(parser))

    // The other stuff.

    // TODO: Operators and whatnot are not supported w/ this current code.
    if(at(parser, TOKEN_ID)) PARSE(0, parse_identifier(parser))
    if(at(parser, TOKEN_NUMERIC)) PARSE(0, parse_literal(parser))
    if(at(parser, TOKEN_STRING)) PARSE(0, parse_literal(parser))

    error_throw("RCT2002", current(parser).loc, "Invalid token %s", TOKEN_NAMES[current(parser).type]);
    step(parser, 1);
    return NULL;
}


void parser_parse(parser_context* parser) {
    node_root* root_node = new_node_root(parser);
    parser->node = (node*)root_node;

    while(current(parser).type != TOKEN_EOF) {
        node* n = parse_statement(parser, 1);
        if(n != NULL)
            list_push(parser, root_node->children, n);
    }
}