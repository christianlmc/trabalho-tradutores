#ifndef AST
#define AST
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

typedef enum TokenType {
    INT_TYPE,
    FLOAT_TYPE,
    ELEM_TYPE,
    SET_TYPE,
    UNDEF_TYPE,
    NA_TYPE,
    ERROR_TYPE
} TokenType;

struct Node {
    TokenType type;
    Node *child;
    Node *next;
    char *value;
};

Node *createNode(char *value);
Node *createNodeWithType(char *value, TokenType type);
TokenType getExpressionType(Node *left, Node *right);
void freeTree(Node *node);
void printTree(Node *node, int level);
void debugNode(Node *node);
#endif