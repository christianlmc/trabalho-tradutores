#ifndef AST
#define AST
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

struct Node {
    Node *child;
    Node *next;
    char *value;
};

Node *createNode(char *value);
void freeTree(Node *node);
void printTree(Node *node, int level);
void debugNode(Node *node);
#endif