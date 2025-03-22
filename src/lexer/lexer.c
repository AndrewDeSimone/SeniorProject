#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

const char *keywords[] = { "var", "into", "if", "while", "print", NULL };

static int is_keyword(const char *str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

Token get_next_token(FILE *input) {
    Token token;
    token.type = TOKEN_UNKNOWN;
    int ch;
    int index = 0;

    // Skip whitespace and comments
    while ((ch = fgetc(input)) != EOF) {
        if (isspace(ch)) continue;
        
        if (ch == '/') {
            int next_ch = fgetc(input);
            if (next_ch == '/') {  // Comment detected
                while ((ch = fgetc(input)) != EOF && ch != '\n');
                continue;
            } else {
                ungetc(next_ch, input);
            }
        }
        break;
    }

    if (ch == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        return token;
    }

    // Identifier or Keyword
    if (isalpha(ch) || ch == '_') {
        index = 0;
        token.value[index++] = ch;
        while (isalnum(ch = fgetc(input)) || ch == '_') {
            if (index < MAX_TOKEN_LENGTH - 1)
                token.value[index++] = ch;
        }
        token.value[index] = '\0';
        ungetc(ch, input);

        token.type = is_keyword(token.value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return token;
    }

    // Number
    if (isdigit(ch)) {
        index = 0;
        token.value[index++] = ch;
        int has_decimal = 0;
        
        while (isdigit(ch = fgetc(input)) || (ch == '.' && !has_decimal)) {
            if (ch == '.') has_decimal = 1;
            if (index < MAX_TOKEN_LENGTH - 1)
                token.value[index++] = ch;
        }
        token.value[index] = '\0';
        ungetc(ch, input);
        
        token.type = TOKEN_NUMBER;
        return token;
    }

    // String literal
    if (ch == '"') {
        index = 0;
        while ((ch = fgetc(input)) != EOF && ch != '"') {
            if (ch == '\\') {  // Handle escape character
                char next_ch = fgetc(input);
                if (next_ch == EOF) break;
                if (index < MAX_TOKEN_LENGTH - 2) {
                    token.value[index++] = '\\';
                    token.value[index++] = next_ch;
                }
            } else if (index < MAX_TOKEN_LENGTH - 1) {
                token.value[index++] = ch;
            }
        }
        token.value[index] = '\0';
        token.type = TOKEN_STRING;
        return token;
    }

    // Operators and Symbols
    char op[3] = { ch, '\0', '\0' };
    
    // Comparative operators
    if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
        int next_ch = fgetc(input);
        if (next_ch == '=') {
            op[1] = '=';
            op[2] = '\0';
            token.type = TOKEN_COMP_OP;
        } else {
            ungetc(next_ch, input);
            token.type = TOKEN_COMP_OP;
        }
        strcpy(token.value, op);
        return token;
    }

    // Logical operators
    if (ch == '&' || ch == '|') {
        int next_ch = fgetc(input);
        if (next_ch == ch) {  // "&&" or "||"
            op[1] = ch;
            token.type = TOKEN_LOGICAL_OP;
            strcpy(token.value, op);
            return token;
        } else {
            ungetc(next_ch, input);
        }
    }

    // Additive operators
    if (ch == '+' || ch == '-') {
        op[0] = ch;
        token.type = TOKEN_ADD_OP;
        strcpy(token.value, op);
        return token;
    }

    // Multiplicative operators
    if (ch == '*' || ch == '/' || ch == '%') {
        op[0] = ch;
        token.type = TOKEN_MULT_OP;
        strcpy(token.value, op);
        return token;
    }

    // Unary operators
    if (ch == '!' || ch == '+' || ch == '-') {
        op[0] = ch;
        token.type = TOKEN_UNARY_OP;
        strcpy(token.value, op);
        return token;
    }

    // Symbols
    if (strchr("{}();", ch)) {
        op[0] = ch;
        token.type = TOKEN_SYMBOL;
        strcpy(token.value, op);
        return token;
    }

    // Unknown token
    op[0] = ch;
    token.type = TOKEN_UNKNOWN;
    strcpy(token.value, op);
    return token;
}

void print_token(Token token) {
    const char *type_names[] = {
        "IDENTIFIER", "NUMBER", "STRING", "KEYWORD",
        "ADD_OP", "MULT_OP", "COMP_OP", "UNARY_OP",
        "LOGICAL_OP", "SYMBOL", "EOF", "UNKNOWN"
    };
    
    printf("Token Type: %s, Value: '%s'\n", type_names[token.type], token.value);
}
