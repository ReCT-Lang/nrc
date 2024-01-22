#pragma once

#include "location.h"

typedef enum token_type_e {
    TOKEN_INVALID,
    TOKEN_EOF,

    TOKEN_KW_VAR,
    TOKEN_KW_PRIVATE,
    TOKEN_KW_SET,
    TOKEN_KW_EXTERN,
    TOKEN_KW_STATIC,
    TOKEN_KW_UNSAFE,
    TOKEN_KW_IF,
    TOKEN_KW_ELSE,
    TOKEN_KW_ELIF,
    TOKEN_KW_FOR,
    TOKEN_KW_FUNCTION,
    TOKEN_KW_RETURN,
    TOKEN_KW_CONTINUE,
    TOKEN_KW_BREAK,
    TOKEN_KW_MAKE,
    TOKEN_KW_CLASS,
    TOKEN_KW_STRUCT,
    TOKEN_KW_ENUM,
    TOKEN_KW_PACKAGE,
    TOKEN_KW_USE,
    TOKEN_KW_ALIAS,
    TOKEN_KW_REF, // For pointer creation, C# style "ref"

    TOKEN_ID,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_NUMERIC,

    TOKEN_TRUE,
    TOKEN_FALSE,

    TOKEN_OP_PLUS,
    TOKEN_OP_MINUS,
    TOKEN_OP_MOD,
    TOKEN_OP_STAR,
    TOKEN_OP_SLASH,
    TOKEN_OP_EQUALS,
    TOKEN_OP_NOT,
    TOKEN_OP_NOT_EQUALS,
    TOKEN_OP_GR_EQUALS,
    TOKEN_OP_LT_EQUALS,
    TOKEN_OP_LT,
    TOKEN_OP_GT,

    TOKEN_COMMA,
    TOKEN_ACCESS, // ->
    TOKEN_ASSIGN,
    TOKEN_ASSIGN_PLUS,
    TOKEN_ASSIGN_MINUS,
    TOKEN_ASSIGN_DIVIDE,
    TOKEN_ASSIGN_MULTIPLY,

    TOKEN_BRACE_OPEN, // {
    TOKEN_BRACE_CLOSE, // }
    TOKEN_BRACKET_OPEN, // [
    TOKEN_BRACKET_CLOSE, // ]
    TOKEN_PARENTHESIS_OPEN, // (
    TOKEN_PARENTHESIS_CLOSE, // )

    TOKEN_PACKAGE_ACCESS,

    TOKEN_END_STMT

} token_type_e;

extern const char* TOKEN_NAMES[];

typedef struct token_t {
    token_type_e type;
    location loc;
    char* data;
} token_t;