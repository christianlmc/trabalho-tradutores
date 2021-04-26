#include "tokens.h"

Token *createToken(char *value, int line, int column) {
    Token *token  = (Token *)malloc(sizeof(Token));
    token->value  = strdup(value);
    token->line   = line;
    token->column = column;
    return token;
}

Token *convertNodeToToken(Node *node) {
    return createToken(node->value, node->line, node->column);
}

void freeToken(Token *token) {
    free(token->value);
    free(token);
}