#include "ast.h"
#include "symbol_table.h"

Node *createNode(char *value) {
    Node *node  = (Node *)malloc(sizeof(Node));
    node->value = malloc(strlen(value) + 1);
    strncpy(node->value, value, strlen(value) + 1);
    node->child = NULL;
    node->next  = NULL;
    node->type  = NA_TYPE;

    return node;
}

Node *createNodeWithType(char *value, TokenType type) {
    Node *node = createNode(value);
    node->type = type;

    return node;
}

TokenType getExpressionType(Node *left, Node *right) {
    TokenType leftType  = left->type;
    TokenType rightType = right->type;

    if (leftType == rightType) {
        return leftType;
    } else {
        if ((leftType == FLOAT_TYPE && rightType == INT_TYPE)
            || (leftType == INT_TYPE && rightType == FLOAT_TYPE)) {
            return FLOAT_TYPE;
        }
    }

    return ERROR_TYPE;
}

void printTree(Node *node, int level) {
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
        printf(RESET);
    } else {
        printf(RED "DEBUG NODE GOT NULL\n" RESET);
    }
}