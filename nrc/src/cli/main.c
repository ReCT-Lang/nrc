#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../errors/error.h"
#include <stdlib.h>

int main() {
    lexer_context* lexer = lexer_create();

    FILE* in = fopen("src/r_test/test_simple.rct", "r");
    lexer_read(lexer, in);
    fclose(in);

    lexer_process(lexer);

    for (int i = 0; i < lexer->token_count; ++i) {
        printf("| %s: %s\n", TOKEN_NAMES[lexer->tokens[i].type], lexer->tokens[i].data);
    }

    parser_context* parser = parser_create(lexer);

    parser_parse(parser);

    lexer_destroy(lexer);

    print_node(parser->node, 0);

    parser_destroy(parser);

    int error_count = enumerate_errors(NULL);
    error* errors = (error*)calloc(sizeof(error), error_count);
    enumerate_errors(errors);

    for (int i = 0; i < error_count; ++i) {
        error e = errors[i];
        printf("[ERR] [L: %u, C: %u] %s: %s\n", e.loc.line, e.loc.column, e.code, e.string);
    }
}