#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKEN_LENGTH 100

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_KEYWORD,
    TOKEN_ADD_OP,            // "+" or "-"
    TOKEN_MULT_OP,           // "*", "/", "%"
    TOKEN_COMP_OP,           // "==", "!=", ">", "<", ">=", "<="
    TOKEN_LOGICAL_OP,        // "&&", "||"
    TOKEN_SYMBOL,            // "{", "}", "(", ")", ";"
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

#endif // TOKEN_H