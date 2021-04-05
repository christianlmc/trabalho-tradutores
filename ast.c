#include "ast.h"
#include <string.h>

Node *createNode(char *value) {
    Node *node  = (Node *)malloc(sizeof(Node));
    node->value = malloc(strlen(value) + 1);
    strncpy(node->value, value, strlen(value) + 1);
    node->child = NULL;
    node->next  = NULL;

    return node;
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
