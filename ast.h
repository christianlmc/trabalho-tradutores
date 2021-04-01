#include <stdio.h>
#include <stdlib.h>

typedef enum Operation {
    ROOT_ENUM,
    FUNCTION_DECLARATION_ENUM,
    EMPTY_ENUM,
    RETURN_ENUM,
    LTE_OP_ENUM,
    GTE_OP_ENUM,
    NEQ_OP_ENUM,
    EQ_OP_ENUM,
    GT_ENUM,
    LT_ENUM,
    INT_LITERAL_ENUM,
    FLOAT_LITERAL_ENUM,
    INT_TYPE_ENUM,
    FLOAT_TYPE_ENUM,
    ELEM_TYPE_ENUM,
    SET_TYPE_ENUM,
} OperationType;

typedef struct Node Node;

struct Node {
    Node *child;
    Node *brother;
    char *value;
    // OperationType op;
};

Node *createNode(char *value);

const char *getOperationType(OperationType type);
void printTree(Node *node, int level);