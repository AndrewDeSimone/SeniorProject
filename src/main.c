#include "lexer/lexer.h"
#include "parser/parser.h" // Include the parser header
#include <stdio.h>
#include <stdlib.h>

void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    // Print indentation for better readability
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            print_ast(node->program.block, indent + 1);
            break;

        case AST_BLOCK:
            printf("Block\n");
            for (int i = 0; node->block.statement[i] != NULL; i++) {
                print_ast(node->block.statement[i], indent + 1);
            }
            break;

        case AST_STATEMENT:
            printf("Statement\n");
            print_ast(node->statement.statement, indent + 1);
            break;

        case AST_DECLARATION:
            printf("Declaration: %s\n", node->declaration.identifier->value);
            break;

        case AST_ASSIGNMENT:
            printf("Assignment: %s\n", node->assignment.identifier->value);
            print_ast(node->assignment.expression, indent + 1);
            break;

        case AST_IF:
            printf("If statement\n");
            print_ast(node->branch.condition, indent + 1);
            print_ast(node->branch.block, indent + 1);
            break;

        case AST_WHILE:
            printf("While statement\n");
            print_ast(node->loop.condition, indent + 1);
            print_ast(node->loop.block, indent + 1);
            break;

        case AST_PRINT:
            printf("Print statement\n");
            print_ast(node->print.expression, indent + 1);
            break;

        case AST_CONDITION:
            printf("Condition: %s\n", node->condition.logicalOperator->token); // Accessing Token's value
            print_ast(node->condition.left, indent + 1);
            print_ast(node->condition.right, indent + 1);
            break;

        case AST_EXPRESSION:
            printf("Expression\n");
            print_ast(node->expression.left, indent + 1);
            print_ast(node->expression.right, indent + 1);
            break;

        case AST_TERM:
            printf("Term\n");
            print_ast(node->term.left, indent + 1);
            print_ast(node->term.right, indent + 1);
            break;

        case AST_FACTOR:
            printf("Factor: %s\n", node->factor.token->value);
            break;

        default:
            printf("Unknown ASTNodeType\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Unable to open input file");
            return EXIT_FAILURE;
        }
    }

    // Parse the program (generate AST from source code)
    ASTNode *ast = parse_program(input);
    
    // Now print the resulting AST for debugging/inspection
    printf("AST for the parsed program:\n");
    print_ast(ast, 0); // Indentation level 0 for root

    if (input != stdin) fclose(input);
    return EXIT_SUCCESS;
}
