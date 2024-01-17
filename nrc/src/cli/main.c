#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../errors/error.h"
#include <stdlib.h>

void throw_exceptions() {
    int error_count = enumerate_errors(NULL);
    error* errors = (error*)calloc(sizeof(error), error_count);
    enumerate_errors(errors);

    for (int i = 0; i < error_count; ++i) {
        error e = errors[i];
        fprintf(stderr, "[ERR] [L: %u, C: %u] %s: %s\n", e.loc.line, e.loc.column, e.code, e.string);
    }
    free(errors);
    if(error_count > 0)
        exit(1);
}

int main() {
    lexer_context* lexer = lexer_create();

    FILE* in = fopen("src/r_test/test_simple.rct", "r");
    lexer_read(lexer, in);
    fclose(in);

    lexer_process(lexer);

    printf("Tokens:\n");
    for (int i = 0; i < lexer->token_count; ++i) {
        printf("| [L: %i, C: %i] %s: %s\n", lexer->tokens[i].loc.line, lexer->tokens[i].loc.column, TOKEN_NAMES[lexer->tokens[i].type], lexer->tokens[i].data);
    }
    printf("\n\n");

    throw_exceptions();

    parser_context* parser = parser_create(lexer);

    parser_parse(parser);

    lexer_destroy(lexer);

    printf("Parser Tree:\n\n");
    print_node(parser->node, NULL, 0);
    printf("\n\n");
    parser_destroy(parser);

    throw_exceptions();
}