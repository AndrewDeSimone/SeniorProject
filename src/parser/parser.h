#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include "ast.h"

void parse(FILE *input);

ASTNode* parse_program(FILE *input);
ASTNode* parse_block(FILE *input);
ASTNode* parse_statement(FILE *input);
ASTNode* parse_declaration(FILE *input);
ASTNode* parse_assignment(FILE *input);
ASTNode* parse_if(FILE *input);
ASTNode* parse_while(FILE *input);
ASTNode* parse_print(FILE *input);
ASTNode* parse_condition(FILE *input);
ASTNode* parse_condition_term(FILE *input);
ASTNode* parse_expression(FILE *input);
ASTNode* parse_term(FILE *input);
ASTNode* parse_factor(FILE *input);
ASTNode* parse_additive_operator(FILE *input);
ASTNode* parse_multiplicative_operator(FILE *input);
ASTNode* parse_logical_operator(FILE *input);

#endif