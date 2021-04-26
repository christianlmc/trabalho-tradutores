#ifndef AST
#define AST
#include "colors.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *createNode(Token *token);
Node *createNodeFromString(char *value);
Node *createNodeWithType(Token *token, TokenType type);
Node *createNodeFromStringWithType(char *value, TokenType type);
void pushNextNode(Node *node, Node *next);
void freeTree(Node *node);
void printTree(Node *node, int level);
void debugNode(Node *node);
#endif