#ifndef SEMANTIC
#define SEMANTIC

#include "ast.h"
#include "colors.h"
#include "symbol_table.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TokenType getExpressionType(Node *left, Node *right);
Node *generateLogicCoercion(Node *left, Node *right);
Node *generateAritmeticCoercion(Node *left, Node *right);
Node *generateArgumentsCoercion(Symbol *scope, Node *functionNode, Node *args);
Node *convertToType(Node *node, TokenType type);
Node *convertToInt(Node *node);
Node *convertToFloat(Node *node);
Node *convertToElem(Node *node);

void checkForPresence(Symbol *symbol, char *id, int line, int column);
#endif