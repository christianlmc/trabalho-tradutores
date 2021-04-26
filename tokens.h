#ifndef TOKENS
#define TOKENS
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

Token *createToken(char *value, int line, int column);
Token *convertNodeToToken(Node *node);
void freeToken(Token *token);
#endif