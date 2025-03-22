#include "lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Unable to open input file");
            return EXIT_FAILURE;
        }
    }

    Token token;
    do {
        token = get_next_token(input);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    if (input != stdin) fclose(input);
    return EXIT_SUCCESS;
}
