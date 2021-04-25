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
    if (left == NULL || right == NULL) return ERROR_TYPE;
    TokenType leftType  = left->type;
    TokenType rightType = right->type;

    if (leftType == rightType) {
        return leftType;
    } else {
        return ERROR_TYPE;
    }
}

Node *generateAritmeticCoercion(Node *left, Node *right) {
    if (left == NULL || right == NULL) return NULL;
    TokenType leftType  = left->type;
    TokenType rightType = right->type;

    if (leftType == rightType && (leftType == INT_TYPE || leftType == FLOAT_TYPE || leftType == ELEM_TYPE)) {
        left->next = right;
        return left;
    } else {
        if ((leftType == FLOAT_TYPE && rightType == INT_TYPE)
            || (leftType == INT_TYPE && rightType == FLOAT_TYPE)) {
            left       = convertToFloat(left);
            right      = convertToFloat(right);
            left->next = right;
            return left;
        } else if ((leftType == ELEM_TYPE && rightType == INT_TYPE)
            || (leftType == INT_TYPE && rightType == ELEM_TYPE)
            || (leftType == ELEM_TYPE && rightType == FLOAT_TYPE)
            || (leftType == FLOAT_TYPE && rightType == ELEM_TYPE)) {
            left       = convertToElem(left);
            right      = convertToElem(right);
            left->next = right;
            return left;
        } else {
            printf(BOLDRED "Error" RESET ": No aritmetic coercion between %s " BOLDWHITE "'%s'" RESET " and %s " BOLDWHITE "'%s'\n" RESET,
                left->value, getTypeName(left->type),
                right->value, getTypeName(right->type));
            left->next = right;
            return left;
        }
    }
}

Node *generateLogicCoercion(Node *left, Node *right) {
    if (left == NULL || right == NULL) return NULL;
    TokenType leftType  = left->type;
    TokenType rightType = right->type;

    if (leftType == rightType) {
        left->next = right;
        return left;
    } else {
        if ((leftType == FLOAT_TYPE && rightType == INT_TYPE)
            || (leftType == INT_TYPE && rightType == FLOAT_TYPE)) {
            left       = convertToFloat(left);
            right      = convertToFloat(right);
            left->next = right;
            return left;
        } else if ((leftType == ELEM_TYPE && rightType == INT_TYPE)
            || (leftType == INT_TYPE && rightType == ELEM_TYPE)
            || (leftType == ELEM_TYPE && rightType == FLOAT_TYPE)
            || (leftType == FLOAT_TYPE && rightType == ELEM_TYPE)) {
            left       = convertToElem(left);
            right      = convertToElem(right);
            left->next = right;
            return left;
        } else {
            printf(BOLDRED "Error" RESET ": No logical coercion between %s " BOLDWHITE "'%s'" RESET " and %s " BOLDWHITE "'%s'\n" RESET,
                left->value, getTypeName(left->type),
                right->value, getTypeName(right->type));
            left->next = right;
            return left;
        }
    }
}

Node *convertToType(Node *node, TokenType type) {
    if (type == INT_TYPE || type == FLOAT_TYPE || type == ELEM_TYPE) {
        if (type == INT_TYPE) {
            node = convertToInt(node);
        } else if (type == FLOAT_TYPE) {
            node = convertToFloat(node);
        } else if (type == ELEM_TYPE) {
            node = convertToElem(node);
        }
    } else {
        if (!(type == SET_TYPE && node->type == SET_TYPE)) {
            printf(BOLDRED "Error" RESET ": Assigment " BOLDWHITE "'%s'" RESET " = " BOLDWHITE "'%s'" RESET " is not possible\n", getTypeName(type), getTypeName(node->type));
        }
    }
    return node;
}

Node *convertToInt(Node *node) {
    Node *coercion;
    if (node != NULL) {
        if (node->type == FLOAT_TYPE) {
            coercion        = createNodeWithType("floatToInt", INT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            coercion        = createNodeWithType("elemToInt", INT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == INT_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error" RESET ": %s (" BOLDWHITE "%s" RESET ") can't be converted to type " BOLDWHITE "int\n" RESET, node->value, getTypeName(node->type));
            return node;
        }
    } else {
        return NULL;
    }
}

Node *convertToFloat(Node *node) {
    Node *coercion;
    if (node != NULL) {
        if (node->type == INT_TYPE) {
            coercion        = createNodeWithType("intToFloat", FLOAT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            coercion        = createNodeWithType("elemToFloat", FLOAT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == FLOAT_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error" RESET ": %s (" BOLDWHITE "%s" RESET ") can't be converted to type " BOLDWHITE "float\n" RESET, node->value, getTypeName(node->type));
            return node;
        }
    } else {
        return NULL;
    }
}

Node *convertToElem(Node *node) {
    Node *coercion;
    if (node != NULL) {
        if (node->type == INT_TYPE) {
            coercion        = createNodeWithType("intToElem", ELEM_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == FLOAT_TYPE) {
            coercion        = createNodeWithType("elemToElem", ELEM_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error" RESET ": %s (" BOLDWHITE "%s" RESET ") can't be converted to type " BOLDWHITE "elem\n" RESET, node->value, getTypeName(node->type));
            return node;
        }
    } else {
        return NULL;
    }
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
        printf(RESET);
    } else {
        printf(RED "DEBUG NODE GOT NULL\n" RESET);
    }
}