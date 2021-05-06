#include "ast.h"

Node *createNode(Token *token) {
    Node *node      = (Node *)malloc(sizeof(Node));
    node->value     = strdup(token->value);
    node->line      = token->line;
    node->column    = token->column;
    node->child     = NULL;
    node->next      = NULL;
    node->type      = NA_TYPE;
    node->tacSymbol = -1;
    strncpy(node->tacType, "$", 2);

    return node;
}

Node *createNodeFromString(char *value) {
    Node *node      = (Node *)malloc(sizeof(Node));
    node->value     = strdup(value);
    node->line      = 0;
    node->column    = 0;
    node->child     = NULL;
    node->next      = NULL;
    node->type      = NA_TYPE;
    node->tacSymbol = -1;
    strncpy(node->tacType, "$", 2);

    return node;
}

Node *createNodeWithType(Token *token, TokenType type) {
    Node *node = createNode(token);
    node->type = type;

    return node;
}

Node *createNodeFromStringWithType(char *value, TokenType type) {
    Node *node = createNodeFromString(value);
    node->type = type;

    return node;
}

void pushNextNode(Node *node, Node *next) {
    if (node == NULL) return;

    while (node->next != NULL) {
        node = node->next;
    }

    node->next = next;
}

void printTree(Node *node, int level) {
    if (node == NULL) return;
    if (level == 0) {
        printf("------------- ABSTRACT SYNTAX TREE -------------\n");
    } else {
        for (int i = 0; i < level; i++)
            printf("--");
        printf(" (level %d) ", level);
    }

    printf("%s", node->value);
    printf("\n");
    if (node->child != NULL) {
        printTree(node->child, level + 1);
    }

    if (node->next != NULL) {
        printTree(node->next, level);
    }
}

void freeTree(Node *node) {
    if (node == NULL) return;
    if (node->child != NULL) {
        freeTree(node->child);
    }

    if (node->next != NULL) {
        freeTree(node->next);
    }
    free(node->value);
    free(node);
}

void debugNode(Node *node) {
    if (node) {
        printf(RED "DEBUG %s:\n", node->value);
        printf("value:      %s\n", node->value);
        printf("type:       %s\n", getTypeName(node->type));
        printf("next:       %d\n", !!node->next);
        printf("child:      %d\n", !!node->child);
        printf("type:       %s\n", getTypeName(node->type));
        printf("tac:        %s%d\n", node->tacType, node->tacSymbol);
        printf(RESET);
    } else {
        printf(RED "DEBUG NODE GOT NULL\n" RESET);
    }
}