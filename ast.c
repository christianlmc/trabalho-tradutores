#include "ast.h"
#include <string.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

Node *createNode(char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    // node->op = op;
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

const char *getOperationType(OperationType type) {
    const char *operationDict[] = {
        "ROOT_ENUM",
        "FUNCTION_DECLARATION_ENUM",
        "EMPTY_ENUM",
        "RETURN_ENUM",
        "LTE_OP_ENUM",
        "GTE_OP_ENUM",
        "NEQ_OP_ENUM",
        "EQ_OP_ENUM",
        "GT_ENUM",
        "LT_ENUM",
        "INT_LITERAL_ENUM",
        "FLOAT_LITERAL_ENUM",
        "INT_TYPE_ENUM",
        "FLOAT_TYPE_ENUM",
        "ELEM_TYPE_ENUM",
        "SET_TYPE_ENUM",
    };

    return operationDict[type];
}
