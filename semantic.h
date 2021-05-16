#ifndef SEMANTIC
#define SEMANTIC

#include "ast.h"
#include "colors.h"
#include "gen_tac.h"
#include "symbol_table.h"
#include "tokens.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern tinyint hasError;
extern int availableTacVar;
extern char *tacCode;

TokenType getExpressionType(Node *left, Node *right);
Node *generateLogicCoercion(Node *left, Node *right);
Node *generateAritmeticCoercion(Node *left, Node *right);
Node *generateArgumentsCoercion(Symbol *scope, Node *functionNode, Node *args);
Node *convertToType(Node *node, TokenType type);
Node *convertToInt(Node *node);
Node *convertToFloat(Node *node);

tinyint hasSameNumberOfArguments(Symbol *functionDef, Node *functionCall, Node *args);
void checkForPresence(Symbol *scope, Token *id);
#endif