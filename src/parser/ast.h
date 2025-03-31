#ifndef AST_H
#define AST_H

#include "../lexer/token.h"

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_STATEMENT,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_IF,
    AST_WHILE,
    AST_PRINT,
    AST_CONDITION,
    AST_CONDITION_TERM,
    AST_EXPRESSION,
    AST_TERM,
    AST_FACTOR,
    AST_UNARY_OPERATOR,
    AST_ADDITIVE_OPERATOR,
    AST_MULTIPLICATIVE_OPERATOR,
    AST_LOGICAL_OPERATOR,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;

    union {
        struct {
            struct ASTNode *block;
        }program;

        struct {
            struct ASTNode **statement;
        }block;

        struct {
            struct ASTNode *statement;
        }statement;

        struct { 
            Token *identifier;
        }declaration;

        struct {
            Token * identifier;
            struct ASTNode *expression;
        }assignment;

        struct {
            struct ASTNode *condition;
            struct ASTNode *block;
        }branch;

        struct {
            struct ASTNode *condition;
            struct ASTNode *block;
        }loop;

        struct {
            struct ASTNode *expression;
        }print;

        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            struct ASTNode *logicalOperator;
        }condition;

        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            struct ASTNode *additiveOperator;
        }expression;

        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            struct ASTNode *multiplicativeOperator;
        }term;

        struct {
            Token *token;
        }factor;

        struct {
            Token *token;
        }logicalOperator;

        struct {
            Token *token;
        }additiveOperator;

        struct {
            Token *token;
        }multiplicativeOperator;

    };
} ASTNode;

#endif