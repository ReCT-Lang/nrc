#include "../lexer/lexer.h"
#include "../parser/parser.h"

int main() {
    lexer_context* lexer = lexer_create();

    FILE* in = fopen("src/r_test/test_simple.rct", "r");
    lexer_process(lexer, in);
    fclose(in);

    for (int i = 0; i < lexer->token_count; ++i) {
        printf("| %s: %s\n", TOKEN_NAMES[lexer->tokens[i].type], lexer->tokens[i].data);
    }

    parser_context* parser = parser_create(lexer);

    parser_parse(parser);

    lexer_destroy(lexer);

    print_node(parser->node, 0);

    parser_destroy(parser);
}