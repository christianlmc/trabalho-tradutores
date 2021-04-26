#include "semantic.h"

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
            printf(BOLDRED "Error" RESET ": No aritmetic coercion between ");
            printf("%s [%d:%d] " BOLDWHITE "'%s'" RESET, left->value, left->line, left->column, getTypeName(left->type));
            printf(" and ");
            printf("%s [%d:%d] " BOLDWHITE "'%s'" RESET, right->value, right->line, right->column, getTypeName(right->type));
            printf("\n");
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
            printf(BOLDRED "Error" RESET ": No logical coercion between ");
            printf("%s [%d:%d] " BOLDWHITE "'%s'" RESET, left->value, left->line, left->column, getTypeName(left->type));
            printf(" and ");
            printf("%s [%d:%d] " BOLDWHITE "'%s'" RESET, right->value, right->line, right->column, getTypeName(right->type));
            printf("\n");
            left->next = right;
            return left;
        }
    }
}

Node *generateArgumentsCoercion(Symbol *scope, Node *functionCall, Node *args) {
    Symbol *functionDef = findSymbolByName(functionCall->value, scope);

    if (functionDef == NULL) {
        return NULL;
    }

    if (!functionDef->isFunction) {
        printf(BOLDRED "Error on %d:%d" RESET ": '%s' is not a function\n", functionDef->line, functionDef->column, functionDef->id);
    } else {
        if (hasSameNumberOfArguments(functionDef, functionCall, args)) {
            Symbol *callArg = functionDef->child;
            Node *auxArgs   = args;
            Node *newArgs   = NULL;

            // Checking arg types
            while (auxArgs != NULL) {
                Token *tokenAux    = convertNodeToToken(auxArgs);
                Node *nodeAux      = createNodeWithType(tokenAux, auxArgs->type);
                Node *convertedArg = convertToType(nodeAux, callArg->type);
                freeToken(tokenAux);
                if (newArgs == NULL) {
                    newArgs = convertedArg;
                } else {
                    pushNextNode(newArgs, convertedArg);
                }
                if (callArg->type != convertedArg->type) {
                    printf(BOLDRED "Error on %d:%d: " RESET, auxArgs->line, auxArgs->column);
                    printf("Expected argument '%s' [%d:%d] ", callArg->id, callArg->line, callArg->column);
                    printf("to be of type " BOLDWHITE "'%s'" RESET ", " BOLDWHITE "'%s'" RESET " given\n", getTypeName(callArg->type), getTypeName(auxArgs->type));
                }
                callArg = callArg->next;
                auxArgs = auxArgs->next;
            }
            freeTree(args);
            return newArgs;
        }
    }

    return args;
}

tinyint hasSameNumberOfArguments(Symbol *functionDef, Node *functionCall, Node *args) {
    int countArgs = 0;

    while (args != NULL) {
        countArgs++;
        args = args->next;
    }

    if (countArgs != functionDef->argsCount) {
        printf(BOLDRED "Error on %d:%d: " RESET, functionCall->line, functionCall->column);
        printf("Function '%s' [%d:%d] expected %d arguments, %d given\n", functionDef->id, functionDef->line, functionDef->column, functionDef->argsCount, countArgs);
    }

    return countArgs == functionDef->argsCount;
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
            printf(BOLDRED "Error on %d:%d: " RESET, node->line, node->column);
            printf("Assigment " BOLDWHITE "'%s'" RESET " = " BOLDWHITE "'%s'" RESET " is not possible\n", getTypeName(type), getTypeName(node->type));
        }
    }
    return node;
}

Node *convertToInt(Node *node) {
    Node *coercion;
    if (node != NULL) {
        if (node->type == FLOAT_TYPE) {
            coercion        = createNodeFromStringWithType("floatToInt", INT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            coercion        = createNodeFromStringWithType("elemToInt", INT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == INT_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error on %d:%d: " RESET, node->line, node->column);
            printf("%s " BOLDWHITE "'%s'" RESET " can't be converted to type " BOLDWHITE "'int'\n" RESET, node->value, getTypeName(node->type));
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
            coercion        = createNodeFromStringWithType("intToFloat", FLOAT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            coercion        = createNodeFromStringWithType("elemToFloat", FLOAT_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == FLOAT_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error on %d:%d: " RESET, node->line, node->column);
            printf("%s " BOLDWHITE "'%s'" RESET " can't be converted to type " BOLDWHITE "'float'\n" RESET, node->value, getTypeName(node->type));
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
            coercion        = createNodeFromStringWithType("intToElem", ELEM_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == FLOAT_TYPE) {
            coercion        = createNodeFromStringWithType("floatToElem", ELEM_TYPE);
            coercion->child = node;
            return coercion;
        } else if (node->type == ELEM_TYPE) {
            return node;
        } else {
            printf(BOLDRED "Error on %d:%d: " RESET, node->line, node->column);
            printf("%s " BOLDWHITE "'%s'" RESET " can't be converted to type " BOLDWHITE "'elem'\n" RESET, node->value, getTypeName(node->type));
            return node;
        }
    } else {
        return NULL;
    }
}

void checkForPresence(Symbol *scope, Token *id) {
    tinyint isPresent = 0;

    Symbol *symbol = getLastChildSymbol(scope);
    // debugSymbol(symbol);
    while (symbol != NULL && !isPresent) {
        if (strcmp(symbol->id, id->value) == 0) {
            isPresent = 1;
            break;
        }

        if (symbol->prev != NULL) {
            symbol = symbol->prev;
        } else {
            symbol = symbol->parent;
        }
    }

    if (!isPresent) {
        printf(BOLDRED "Error on %d:%d" RESET ": '%s' undeclared\n", id->line, id->column, id->value);
    }
}