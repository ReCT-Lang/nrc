#include "lexer.h"
#include <stdlib.h>
#include "token.h"
#include <string.h>

// I am terribly sorry...
// But nevertheless, fucking lexer let's goooo!!!!

// Push a token to the lexer context array
static void push_token(lexer_context* lexer, token_t token) {
    if(lexer->tokens_allocated <= lexer->token_count + 1) {
        lexer->tokens = reallocarray(lexer->tokens, lexer->tokens_allocated * 2 + 1, sizeof(lexer_context));
        lexer->tokens_allocated = lexer->tokens_allocated * 2 + 1;
    }
    lexer->tokens[lexer->token_count] = token;
    lexer->token_count++;
}

// Push a token with only a type
static void push_empty_token(lexer_context* lexer, token_type_e type) {
    token_t token;
    token.type = type;
    token.data = NULL;
    push_token(lexer, token);
}

// Initialize a lexer context
lexer_context* lexer_create() {
    lexer_context* lexer = (lexer_context*)malloc(sizeof(lexer_context));
    lexer->token_count = 0;
    lexer->tokens_allocated = 0;
    lexer->tokens = NULL;
    return lexer;
}

// Delete a lexer context
void lexer_destroy(lexer_context* context) {
    for (int i = 0; i < context->token_count; ++i) {
        free(context->tokens[i].data);
    }

    free(context);
}

static int proc_long_simple(lexer_context* context, const char* str, FILE* file, int c) {
    if(c != str[0])
        return 0;

    int len = (int)strlen(str);
    char* r_buf = (char*)malloc(len + 1);
    r_buf[0] = (char)c;

    for (int i = 1; i < len; ++i) {
        char got = (char)fgetc(file);
        r_buf[i] = got;
    }

    // We need to add back everything to the file in reverse order(?).
    // Otherwise we'll have issues!
    for (int i = len - 1; i >= 1; --i) {
        if(ungetc(r_buf[i], file) != r_buf[i]) {
            fprintf(stderr, "Could not push char back onto lookahead stream!\n");
            exit(EXIT_FAILURE);
        }
    }
    r_buf[len] = 0;

    int eq = strcmp(r_buf, str);

    // If it's equal, we'll consume a number of tokens :)
    if(eq == 0) {
        for (int i = 1; i < len; ++i) {
            fgetc(file);
        }
    }

    free(r_buf); // Memory leek
    return eq == 0 ? 1 : 0;
}

// Just peek to the next char
static int look_ahead(FILE* file) {
    int c = fgetc(file);
    if(ungetc(c, file) != c) {
        fprintf(stderr, "Could not push char back onto lookahead stream!\n");
        exit(EXIT_FAILURE);
    }
    return c;
}

static int is_char_numeric(int c) {
    // Presuming we use ASCII here...
    return c >= '0' && c <= '9';
}

// We allow A-Z in any capitalization plus underscores in the start of a word/identifier
static int is_char_word_starter(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}
// Then 2nd character onward allows us to do numbers tool.
static int is_char_word(int c) {
    return is_char_word_starter(c) || is_char_numeric(c);
}

// This monster is responsible for tokenizing strings.
static void lex_string(lexer_context* context, FILE* file) {
    // Average sting would be max 32 chars I'd guess
    char* read_buffer = malloc(32);
    // Due to performance concerns, we'll be using squared list growth(or whatever it is called).
    // Since read_buffer doesn't grow every iteration, we need to keep track of the allocated size.
    int length = 0;
    int buffer_size = 32;

    while (1) {
        int c = fgetc(file);
        if(c == EOF) {
            free(read_buffer);
            fprintf(stderr, "Unexpected EOF\n");
            return;
        }
        if(c == '\n') {
            free(read_buffer);
            fprintf(stderr, "Unexpected EOF\n");
            return;
        }
        if(c == '"') break;

        // All right, time to add on this character.
        // First we make sure there's enough space for us.
        if(buffer_size <= length + 1) {
            // If there isn't, reallocate the buffer with double size.
            char* new = realloc(read_buffer, buffer_size * 2);
            if(new == NULL) {
                // Ff we can't reallocate we just throw an error and quit.
                fprintf(stderr, "String read buffer could not be reallocated\n");
                free(read_buffer);
                exit(EXIT_FAILURE);
            }
            // Otherwise we set our read buffer to the resized one and update our variables.
            read_buffer = new;
            buffer_size *= 2;
        }
        // Then we can add our character
        read_buffer[length] = (char)c;
        length++;
    }

    // We might not need this resizing pass, since our buffer might already
    // be big enough, but just to be safe. I'll have to take a look at this :)
    // (preferably I'd like to cut this piece out since it's duplicate code)
    if(buffer_size <= length + 1) {
        char* new = realloc(read_buffer, buffer_size * 2);
        if(new == NULL) {
            fprintf(stderr, "String read buffer could not be reallocated\n");
            free(read_buffer);
            exit(EXIT_FAILURE);
        }
        read_buffer = new;
    }
    read_buffer[length] = 0;

    // Then we build our token and push it.
    // DON'T FREE THE READ BUFFER!!!
    token_t token;
    token.data = read_buffer;
    token.type = TOKEN_STRING;

    push_token(context, token);
}

// We treat ourselves to duplicate code for word tokenization.
static void lex_word(lexer_context* context, FILE* file, int first) {
    // Average word would be 32 chars I'd guess
    char* read_buffer = malloc(32);
    // Due to performance concerns, we'll be using squared list growth(or whatever it is called).
    // Since read_buffer doesn't grow every iteration, we need to keep track of the allocated
    // size.
    int length = 0;
    int buffer_size = 32;

    read_buffer[0] = (char)first;
    length++;

    int c;
    while (1) {
        c = fgetc(file);
        if(!is_char_word(c)) break;

        // All right, time to add on this character.
        // First we make sure there's enough space for us.
        if(buffer_size <= length + 1) {
            // If there isn't, reallocate the buffer with double size.
            char* new = realloc(read_buffer, buffer_size * 2);
            if(new == NULL) {
                // Ff we can't reallocate we just throw an error and quit.
                fprintf(stderr, "Word read buffer could not be reallocated\n");
                free(read_buffer);
                exit(EXIT_FAILURE);
            }
            // Otherwise we set our read buffer to the resized one and update our variables.
            read_buffer = new;
            buffer_size *= 2;
        }
        // Then we can add our character
        read_buffer[length] = (char)c;
        length++;
    }

    // Since we get the character after the word and then cancel, we'll have to put it back
    // into the file to be able to then lex it in the future.

    // This does not apply to strings since they are terminated by the quotes, which we'll throw
    // away anyway.
    ungetc(c, file);

    // We might not need this resizing pass, since our buffer might already
    // be big enough, but just to be safe. I'll have to take a look at this :)
    // (preferably I'd like to cut this piece out since it's duplicate code)
    if(buffer_size <= length + 1) {
        char* new = realloc(read_buffer, buffer_size * 2);
        if(new == NULL) {
            fprintf(stderr, "Word read buffer could not be reallocated\n");
            free(read_buffer);
            exit(EXIT_FAILURE);
        }
        read_buffer = new;
    }
    read_buffer[length] = 0;

    // Then we build our token and push it.
    // DON'T FREE THE READ BUFFER!!!
    token_t token;
    token.data = read_buffer;
    token.type = TOKEN_ID;

    push_token(context, token);
}

// Imagine if we had a template for this and not just ctrl+c ctrl+v
// See comments in lex_string and lex_word where missing!

static void lex_numeric(lexer_context* context, FILE* file, int first) {
    // Average numeric would be 8 chars I'd guess, instead of 32.
    char* read_buffer = malloc(8);
    int length = 0;
    int buffer_size = 8;

    read_buffer[0] = (char)first;
    length++;

    int c;
    while (1) {
        c = fgetc(file);

        // TODO: Decimals fucking suck.
        if(!is_char_numeric(c)) break;
        if(buffer_size <= length + 1) {
            char* new = realloc(read_buffer, buffer_size * 2);
            if(new == NULL) {
                fprintf(stderr, "Numeric read buffer could not be reallocated\n");
                free(read_buffer);
                exit(EXIT_FAILURE);
            }

            read_buffer = new;
            buffer_size *= 2;
        }

        read_buffer[length] = (char)c;
        length++;
    }

    ungetc(c, file);

    if(buffer_size <= length + 1) {
        char* new = realloc(read_buffer, buffer_size * 2);
        if(new == NULL) {
            fprintf(stderr, "Numeric read buffer could not be reallocated\n");
            free(read_buffer);
            exit(EXIT_FAILURE);
        }
        read_buffer = new;
    }
    read_buffer[length] = 0;

    // Then we build our token and push it.
    // DON'T FREE THE READ BUFFER!!!
    token_t token;
    token.data = read_buffer;
    token.type = TOKEN_NUMERIC;

    push_token(context, token);
}

// Characters we don't care about, such as whitespace
#define CASE_SKIP(_char) if(c == _char) continue;
// Single-character tokens
#define CASE_SIMPLE(_char, _type) if(c == _char) { push_empty_token(context, _type); continue; }
// Multi-character tokens WITHOUT any attached data
#define CASE_SIMPLE_MULT(_str, _type) { if(proc_long_simple(context, _str, file, c)) { push_empty_token(context, _type); continue; } }

void lexer_process(lexer_context* context, FILE* file) {

    int c;
    while((c = fgetc(file)) != EOF) {
        // Time to iterate through a million different cases.

        if(c == '/' && look_ahead(file) == '/') {
            while (c != '\n' && c != EOF) c = fgetc(file);
            continue;
        }

        CASE_SKIP(' ')
        CASE_SKIP('\t')
        CASE_SKIP('\n')
        CASE_SKIP('\r')
        CASE_SIMPLE(';', TOKEN_END_STMT)

        CASE_SIMPLE('(', TOKEN_PARENTHESIS_OPEN)
        CASE_SIMPLE(')', TOKEN_PARENTHESIS_CLOSE)

        CASE_SIMPLE('{', TOKEN_BRACE_OPEN)
        CASE_SIMPLE('}', TOKEN_BRACE_CLOSE)

        CASE_SIMPLE('[', TOKEN_BRACKET_OPEN)
        CASE_SIMPLE(']', TOKEN_BRACKET_CLOSE)

        CASE_SIMPLE_MULT("::", TOKEN_PACKAGE_ACCESS)

        // Characters like minus and "<" need to be checked in order of size.
        // If we didn't, we might get [TOKEN_OP_LT, TOKEN_OP_MINUS] when we meant [TOKEN_ASSIGN].

        CASE_SIMPLE_MULT("->", TOKEN_ACCESS)

        CASE_SIMPLE_MULT("<-", TOKEN_ASSIGN)
        CASE_SIMPLE_MULT("<-/", TOKEN_ASSIGN_DIVIDE)
        CASE_SIMPLE_MULT("<-*", TOKEN_ASSIGN_MULTIPLY)
        CASE_SIMPLE_MULT("<--", TOKEN_ASSIGN_MINUS)
        CASE_SIMPLE_MULT("<-+", TOKEN_ASSIGN_PLUS)

        CASE_SIMPLE_MULT("<=", TOKEN_OP_LT_EQUALS)
        CASE_SIMPLE_MULT(">=", TOKEN_OP_GR_EQUALS)

        CASE_SIMPLE('=', TOKEN_OP_EQUALS)

        CASE_SIMPLE('<', TOKEN_OP_LT)
        CASE_SIMPLE('>', TOKEN_OP_GT)

        CASE_SIMPLE('+', TOKEN_OP_PLUS)
        CASE_SIMPLE('-', TOKEN_OP_MINUS)
        CASE_SIMPLE('*', TOKEN_OP_STAR)
        CASE_SIMPLE('/', TOKEN_OP_SLASH)

        if(c == '"') { lex_string(context,file); continue; }
        if(is_char_word_starter(c)) { lex_word(context, file, c); continue; }
        if(is_char_numeric(c)) { lex_numeric(context, file, c); continue; }
        // For now, we just get mad :)
        fprintf(stderr, "Invalid token %2X(%c)\n", c, c);
    }
    push_empty_token(context, TOKEN_EOF);
}