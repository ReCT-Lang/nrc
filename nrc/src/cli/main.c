#include "../lexer/lexer.h"

int main() {
    lexer_context* lexer = lexer_create();

    FILE* in = fopen("src/r_test/test_simple.rct", "r");
    lexer_process(lexer, in);
    fclose(in);

    lexer_destroy(lexer);
}