#include "parser.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

// Utility function to get the next token and handle errors
Token expect_token(FILE *input, TokenType expected) {
    Token token = get_next_token(input);
    if (token.type != expected) {
        printf("Syntax error: Expected %d, found %d\n", expected, token.type);
        exit(1);  // Or handle error as appropriate
    }
    return token;
}

// Parse program: <program> = <block>
ASTNode* parse_program(FILE *input) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node -> type = AST_PROGRAM;
    node -> program.block = parse_block(input);
    return node;  
}

// Parse block: <block> = { <statement>; }
ASTNode* parse_block(FILE *input) {
    // Create a new AST node for the block
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    
    // Dynamically allocate an array of AST nodes to store statements
    node->block.statement = malloc(sizeof(ASTNode*) * 10);  // Allocate space for up to 10 statements (expandable)
    int statement_count = 0;
    
    // Parse statements in the block
    while (1) {
        // Get the next token to decide if we've reached the end of the block or need to continue parsing
        Token token = get_next_token(input);

        // If we've reached the end of the file or an unexpected token, stop parsing
        if (token.type == TOKEN_EOF || token.type == TOKEN_UNKNOWN) {
            break;
        }
        
        // Go back one token because we're processing a statement
        ungetc(token.value[0], input);
        
        // Parse the statement and store it in the block
        ASTNode *statement_node = parse_statement(input);
        node->block.statement[statement_count++] = statement_node;
        
        // For simplicity, the array size is hardcoded to 10 statements
        // You can expand this later if needed (e.g., dynamically resize the array)
    }
    
    return node;
}


// Parse statement: <statement> = <declaration> | <assignment> | <if> | <while> | <print>
ASTNode* parse_statement(FILE *input) {
    // Get the next token to decide the type of the statement
    Token token = get_next_token(input);
    
    // If we encounter EOF or an unknown token, we should not proceed
    if (token.type == TOKEN_EOF || token.type == TOKEN_UNKNOWN) {
        return NULL; // No valid statement
    }

    // Handle the case of a declaration (i.e., "var <identifier>")
    if (token.type == TOKEN_KEYWORD && strcmp(token.value, "var") == 0) {
        return parse_declaration(input);
    }

    // Handle assignment (i.e., <identifier> "into" <expression>)
    if (token.type == TOKEN_IDENTIFIER) {
        ungetc(token.value[0], input); // put the token back because we'll process it in parse_assignment
        return parse_assignment(input);
    }

    // Handle "if" statement
    if (token.type == TOKEN_KEYWORD && strcmp(token.value, "if") == 0) {
        return parse_if(input);
    }

    // Handle "while" statement
    if (token.type == TOKEN_KEYWORD && strcmp(token.value, "while") == 0) {
        return parse_while(input);
    }

    // Handle "print" statement
    if (token.type == TOKEN_KEYWORD && strcmp(token.value, "print") == 0) {
        return parse_print(input);
    }

    // If none of the above conditions match, return NULL or handle as an error
    return NULL; // Invalid statement
}

// Parse declaration: <declaration> = "var" identifier
ASTNode* parse_declaration(FILE *input) {
    // Create a new ASTNode for the declaration
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_DECLARATION;

    // Get the next token, which should be the identifier
    Token token = get_next_token(input);

    // Ensure the token is an identifier
    if (token.type != TOKEN_IDENTIFIER) {
        // Handle error: Expected an identifier after 'var'
        free(node);
        return NULL;  // Error: Expected an identifier
    }

    // Store the identifier in the ASTNode (pointing to the Token)
    node->declaration.identifier = malloc(sizeof(Token));
    *node->declaration.identifier = token;

    return node;
}


// Parse assignment: <assignment> = identifier "into" <expression>
ASTNode* parse_assignment(FILE *input) {
    // Create a new ASTNode for the assignment
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;

    // Get the next token, which should be an identifier
    Token token = get_next_token(input);

    // Ensure the token is an identifier
    if (token.type != TOKEN_IDENTIFIER) {
        // Handle error: Expected an identifier
        free(node);
        return NULL;  // Error: Expected an identifier
    }

    // Store the identifier in the ASTNode (pointing to the Token)
    node->assignment.identifier = malloc(sizeof(Token));
    *node->assignment.identifier = token;

    // Get the next token, which should be "into"
    token = get_next_token(input);
    if (token.type != TOKEN_KEYWORD || strcmp(token.value, "into") != 0) {
        // Handle error: Expected "into"
        free(node->assignment.identifier);
        free(node);
        return NULL;  // Error: Expected 'into' keyword
    }

    // Parse the expression after "into"
    node->assignment.expression = parse_expression(input);
    if (node->assignment.expression == NULL) {
        // Handle error: Failed to parse expression
        free(node->assignment.identifier);
        free(node);
        return NULL;  // Error: Failed to parse expression
    }

    return node;
}


// Parse if statement: <if> = "if" <condition> "{" <block> "}"
ASTNode* parse_if(FILE *input) {
    // Create a new ASTNode for the if statement
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_IF;

    // Get the next token, which should be the "if" keyword
    Token token = get_next_token(input);
    if (token.type != TOKEN_KEYWORD || strcmp(token.value, "if") != 0) {
        // Handle error: Expected "if"
        free(node);
        return NULL;  // Error: Expected 'if' keyword
    }

    // Parse the condition for the "if"
    node->branch.condition = parse_condition(input);
    if (node->branch.condition == NULL) {
        // Handle error: Failed to parse condition
        free(node);
        return NULL;  // Error: Failed to parse condition
    }

    // Get the next token, which should be the opening brace '{'
    token = get_next_token(input);
    if (token.type != TOKEN_SYMBOL || strcmp(token.value, "{") != 0) {
        // Handle error: Expected opening '{'
        free(node->branch.condition);
        free(node);
        return NULL;  // Error: Expected '{' symbol
    }

    // Parse the block inside the "if"
    node->branch.block = parse_block(input);
    if (node->branch.block == NULL) {
        // Handle error: Failed to parse block
        free(node->branch.condition);
        free(node);
        return NULL;  // Error: Failed to parse block
    }

    // Return the fully constructed ASTNode
    return node;
}

// Parse while statement: <while> = "while" <condition> "{" <block> "}"
ASTNode* parse_while(FILE *input) {
    // Create a new ASTNode for the while statement
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;

    // Get the next token, which should be the "while" keyword
    Token token = get_next_token(input);
    if (token.type != TOKEN_KEYWORD || strcmp(token.value, "while") != 0) {
        // Handle error: Expected "while"
        free(node);
        return NULL;  // Error: Expected 'while' keyword
    }

    // Parse the condition for the "while"
    node->loop.condition = parse_condition(input);
    if (node->loop.condition == NULL) {
        // Handle error: Failed to parse condition
        free(node);
        return NULL;  // Error: Failed to parse condition
    }

    // Get the next token, which should be the opening brace '{'
    token = get_next_token(input);
    if (token.type != TOKEN_SYMBOL || strcmp(token.value, "{") != 0) {
        // Handle error: Expected opening '{'
        free(node->loop.condition);
        free(node);
        return NULL;  // Error: Expected '{' symbol
    }

    // Parse the block inside the "while"
    node->loop.block = parse_block(input);
    if (node->loop.block == NULL) {
        // Handle error: Failed to parse block
        free(node->loop.condition);
        free(node);
        return NULL;  // Error: Failed to parse block
    }

    // Return the fully constructed ASTNode
    return node;
}


// Parse print statement: <print> = "print" <expression>
ASTNode* parse_print(FILE *input) {
    // Create a new ASTNode for the print statement
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_PRINT;

    // Get the next token, which should be the "print" keyword
    Token token = get_next_token(input);
    if (token.type != TOKEN_KEYWORD || strcmp(token.value, "print") != 0) {
        // Handle error: Expected "print"
        free(node);
        return NULL;  // Error: Expected 'print' keyword
    }

    // Parse the expression for the print statement
    node->print.expression = parse_expression(input);
    if (node->print.expression == NULL) {
        // Handle error: Failed to parse expression
        free(node);
        return NULL;  // Error: Failed to parse expression
    }

    // Return the fully constructed ASTNode for the print statement
    return node;
}


// Parse condition: <condition> = <condition term> { ( "&&" | "||" ) <condition term> }
ASTNode* parse_condition(FILE *input) {
    // Create a new ASTNode for the condition
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_CONDITION;

    // Parse the first condition term (either ( <condition> ) or <expression> <comparative operator> <expression>)
    node->condition.left = parse_condition_term(input);
    if (node->condition.left == NULL) {
        // Handle error: Failed to parse first condition term
        free(node);
        return NULL;
    }

    // Parse the rest of the condition terms (if any) with logical operators (&& or ||)
    Token token = get_next_token(input);
    while (token.type == TOKEN_LOGICAL_OP && (strcmp(token.value, "&&") == 0 || strcmp(token.value, "||") == 0)) {
        // Logical operator: either "&&" or "||"
        node->condition.logicalOperator = malloc(strlen(token.value) + 1);
        strcpy(node->condition.logicalOperator, token.value);

        // Parse the next condition term
        ASTNode *right_term = parse_condition_term(input);
        if (right_term == NULL) {
            // Handle error: Failed to parse right condition term
            free(node);
            return NULL;
        }

        // Update the left side of the condition to the newly parsed term
        ASTNode *new_node = malloc(sizeof(ASTNode));
        new_node->type = AST_CONDITION;
        new_node->condition.left = node->condition.left;
        new_node->condition.logicalOperator = node->condition.logicalOperator;
        new_node->condition.right = right_term;
        node = new_node;

        // Get the next token to check for further logical operators
        token = get_next_token(input);
    }

    // Return the fully constructed ASTNode for the condition
    return node;
}


// Parse condition term: <condition term> = "(" <condition> ")" | <expression> <comparative operator> <expression>
ASTNode* parse_condition_term(FILE *input) {
    Token token = get_next_token(input);
    
    // Case 1: Parenthesized condition
    if (token.type == TOKEN_SYMBOL && token.value[0] == '(') {
        // Parse the inner condition
        ASTNode *node = parse_condition(input);
        
        // Expecting a closing parenthesis
        token = get_next_token(input);
        if (token.type != TOKEN_SYMBOL || token.value[0] != ')') {
            // Handle error: missing closing parenthesis
            free(node);
            return NULL;
        }

        return node;  // Return the parsed condition
    }

    // Case 2: Expression with comparative operator
    ASTNode *left_expr = parse_expression(input);
    if (!left_expr) return NULL;  // Error if we can't parse the left expression
    
    // Get the comparative operator (e.g., ==, !=, <, >, <=, >=)
    token = get_next_token(input);
    if (token.type != TOKEN_COMP_OP) {
        // Handle error: expected comparative operator
        free(left_expr);
        return NULL;
    }
    
    // Parse the right expression
    ASTNode *right_expr = parse_expression(input);
    if (!right_expr) {
        // Handle error: expected right expression
        free(left_expr);
        return NULL;
    }

    // Create a new AST node for the condition term (comparative expression)
    ASTNode *condition_term_node = malloc(sizeof(ASTNode));
    condition_term_node->type = AST_CONDITION_TERM;
    condition_term_node->condition.left = left_expr;
    condition_term_node->condition.logicalOperator = strdup(token.value);  // store the operator as a string
    condition_term_node->condition.right = right_expr;

    return condition_term_node;
}


// Parse expression: <expression> = <term> { <additive operator> <term> }
ASTNode* parse_expression(FILE *input) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_EXPRESSION;

    // Start by parsing the first term in the expression
    node->expression.left = parse_term(input);
    if (!node->expression.left) {
        // Error: failed to parse the first term
        free(node);
        return NULL;
    }

    // Look for additive operators (+ or -) and continue parsing terms
    Token token = get_next_token(input);
    while (token.type == TOKEN_ADD_OP && (strcmp(token.value, "+") == 0 || strcmp(token.value, "-") == 0)) {
        // Store the operator
        node->expression.additiveOperator = strdup(token.value);

        // Parse the next term in the expression
        ASTNode *right_term = parse_term(input);
        if (!right_term) {
            // Error: failed to parse the next term
            free(node);
            return NULL;
        }

        // Create a new AST node for the expression, linking left and right with the operator
        ASTNode *new_node = malloc(sizeof(ASTNode));
        new_node->type = AST_EXPRESSION;
        new_node->expression.left = node->expression.left;
        new_node->expression.additiveOperator = node->expression.additiveOperator;
        new_node->expression.right = right_term;

        // Update the node to point to the newly created node
        node = new_node;

        // Get the next token (check for further additive operators or the end of the expression)
        token = get_next_token(input);
    }

    return node;
}


// Parse term: <term> = <factor> { <multiplicative operator> <factor> }
ASTNode* parse_term(FILE *input) {
    ASTNode *node = malloc(sizeof(ASTNode)); // Create a new AST node for the term.
    node->type = AST_TERM;

    // Start by parsing the first factor in the term
    node->term.left = parse_factor(input);
    if (!node->term.left) {
        // Error: Failed to parse the first factor
        free(node);
        return NULL;
    }

    // Look for multiplicative operators (*, /, %)
    Token token = get_next_token(input);
    while (token.type == TOKEN_MULT_OP && 
           (strcmp(token.value, "*") == 0 || 
            strcmp(token.value, "/") == 0 || 
            strcmp(token.value, "%") == 0)) {

        // Store the operator in the AST node
        node->term.multiplicativeOperator = strdup(token.value); // Assign the operator (e.g., "*", "/", "%")

        // Parse the next factor after the operator
        ASTNode *right_factor = parse_factor(input);
        if (!right_factor) {
            // Error: Failed to parse the next factor after operator
            free(node);
            return NULL;
        }

        // Create a new node to combine the left side (current factor) and the new right factor
        ASTNode *new_node = malloc(sizeof(ASTNode));
        new_node->type = AST_TERM;
        new_node->term.left = node->term.left;
        new_node->term.multiplicativeOperator = node->term.multiplicativeOperator;  // Reuse the operator
        new_node->term.right = right_factor;  // Link the right factor

        // Update the original node to point to the newly created node, which represents the current state of the term
        node = new_node;

        // Get the next token (we're checking for another multiplicative operator or the end of the term)
        token = get_next_token(input);
    }

    return node; // Return the resulting term AST node
}


// Parse factor: <factor> = number | identifier
ASTNode* parse_factor(FILE *input) {
    Token token = get_next_token(input);  // Get the next token

    ASTNode *node = malloc(sizeof(ASTNode));  // Create a new AST node
    node->type = AST_FACTOR;  // Set the node type to factor

    // Check if the token is a number
    if (token.type == TOKEN_NUMBER) {
        node->factor.token = malloc(sizeof(Token));  // Allocate memory for the token
        *node->factor.token = token;  // Store the number token in the AST node
        return node;
    }

    // Check if the token is an identifier
    if (token.type == TOKEN_IDENTIFIER) {
        node->factor.token = malloc(sizeof(Token));  // Allocate memory for the token
        *node->factor.token = token;  // Store the identifier token in the AST node
        return node;
    }

    // If the token is neither a number nor an identifier, it's an error
    fprintf(stderr, "Error: Expected number or identifier, but got '%s'\n", token.value);
    free(node);
    return NULL;
}

// Parse additive operator: <additive operator> = "+" | "-"
ASTNode* parse_additive_operator(FILE *input) {
    Token token = get_next_token(input); // Get the next token from the lexer

    if (token.type == TOKEN_ADD_OP) {
        // If the token is an additive operator (+ or -), create an AST node
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = AST_ADDITIVE_OPERATOR;  // Set the node type
        node->additiveOperator.token = malloc(sizeof(Token));  // Allocate memory for the token
        *node->additiveOperator.token = token;  // Copy the token into the node
        return node;  // Return the node
    }

    // If we reach here, something went wrong (should be an additive operator)
    printf("Error: Expected additive operator (+ or -), but got %s\n", token.value);
    return NULL;  // Return NULL if the token wasn't a valid additive operator
}



// Parse multiplicative operator: <multiplicative operator> = "*" | "/" | "%"
ASTNode* parse_multiplicative_operator(FILE *input) {
    Token token = get_next_token(input); // Get the next token from the lexer

    if (token.type == TOKEN_MULT_OP) {
        // If the token is an additive operator (+ or -), create an AST node
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = AST_MULTIPLICATIVE_OPERATOR;  // Set the node type
        node->multiplicativeOperator.token = malloc(sizeof(Token));  // Allocate memory for the token
        *node->multiplicativeOperator.token = token;  // Copy the token into the node
        return node;  // Return the node
    }

    // If we reach here, something went wrong (should be an additive operator)
    printf("Error: Expected multiplicative operator (* or /), but got %s\n", token.value);
    return NULL;  // Return NULL if the token wasn't a valid additive operator
}


// Parse comparative operator: <comparative operator> = "==" | "!=" | ">" | "<" | ">=" | "<="
ASTNode* parse_logical_operator(FILE *input) {
    Token token = get_next_token(input); // Get the next token from the lexer

    if (token.type == TOKEN_LOGICAL_OP) {
        // If the token is an additive operator (+ or -), create an AST node
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = AST_LOGICAL_OPERATOR;  // Set the node type
        node->logicalOperator.token = malloc(sizeof(Token));  // Allocate memory for the token
        *node->logicalOperator.token = token;  // Copy the token into the node
        return node;  // Return the node
    }

    // If we reach here, something went wrong (should be an additive operator)
    printf("Error: Expected logical operator (* or /), but got %s\n", token.value);
    return NULL;  // Return NULL if the token wasn't a valid additive operator
}
