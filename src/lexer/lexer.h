#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

// Retrieve the next token from the input
Token get_next_token(FILE *input);

// Print token details (for debugging)
void print_token(Token token);

#endif // LEXER_H