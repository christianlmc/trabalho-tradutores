#include "symbol_table.h"

Symbol *createSymbol(char *type, char *id, tinyint isBlock, Node *node) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    int argsCount  = 0;

    symbol->isFunction = node != NULL;
    symbol->id         = malloc(strlen(id) + 1);
    strncpy(symbol->id, id, strlen(id) + 1);
    symbol->type    = getSymbolTypeByName(type);
    symbol->isBlock = isBlock;
    symbol->next    = NULL;
    symbol->prev    = NULL;
    symbol->parent  = NULL;
    symbol->child   = NULL;

    while (node != NULL) {
        Node *nodeChild = node->child;

        if (nodeChild) {
            char *type = nodeChild->value;
            char *id   = nodeChild->next->value;
            argsCount++;
            pushChildSymbol(symbol, createSymbol(type, id, 0, NULL));
        }
        node = node->next;
    }

    symbol->argsCount = argsCount;

    return symbol;
}

Symbol *createGlobalSymbol() {
    return createSymbol("N/A", "N/A", 0, NULL);
}

void pushChildSymbol(Symbol *symbol, Symbol *child) {
    printf("Pushing %s to be child of %s\n", child->id, symbol->id);
    child->parent = symbol;
    if (symbol->child == NULL) {
        symbol->child = child;
    } else {
        pushNextSymbol(symbol->child, child);
    }
}

void pushNextSymbol(Symbol *symbol, Symbol *next) {
    while (symbol->next != NULL) {
        symbol = symbol->next;
    }

    symbol->next = next;
    next->prev   = symbol;
}

Symbol *createBlock() {
    return createSymbol("N/A", "N/A", 1, NULL);
}

void debugSymbol(Symbol *symbol) {
    printf(RED "DEBUG %s:\n", symbol->id);
    printf("id:         %s\n", symbol->id);
    printf("type:       %s\n", getSymbolTypeName(symbol->type));
    printf("isFunction: %d\n", symbol->isFunction);
    printf("argsCount:  %d\n", symbol->argsCount);
    printf("isBlock:    %d\n", symbol->isBlock);
    printf("next:       %d\n", !!symbol->next);
    printf("prev:       %d\n", !!symbol->prev);
    printf("parent:     %d\n", !!symbol->parent);
    printf("child:      %d\n", !!symbol->child);
    printf(RESET);
}

void printSymbolTable(Symbol *symbol, int level) {
    if (level == 0) {
        printf("------------- SYMBOL TABLE -------------\n");
    }
    if (symbol->type != NA_S) {
        printf("%-8s%-32s%-12s%-8s%s\n", "TYPE", "SYMBOL", "FUNCTION", "BLOCK", "ARGS COUNT");
    }

    while (symbol != NULL) {
        if (symbol->type != NA_S) {
            printf("%-8s%-32s%-12s%-8s%d\n", getSymbolTypeName(symbol->type), symbol->id, convertToBoolean(symbol->isFunction), convertToBoolean(symbol->isBlock), symbol->argsCount);
        }
        if (symbol->child != NULL) {
            printf("------------- SYMBOL TABLE (%s) -------------\n", symbol->type == NA_S ? "N/A" : symbol->id);
            printSymbolTable(symbol->child, level + 1);
            printf("------------- END SYMBOL TABLE (%s) -------------\n", symbol->type == NA_S ? "N/A" : symbol->id);
        }
        symbol = symbol->next;
    }
}

char *convertToBoolean(tinyint boolean) {
    return boolean ? "Yes" : "No";
}

void freeSymbolTable(SymbolTable *table) {
    Symbol *aux = table->first;
    Symbol *aux2;

    while (aux != NULL) {
        aux2 = aux->next;
        free(aux->id);
        free(aux);
        aux = aux2;
    }

    free(table);
}

const char *getSymbolTypeName(SymbolType type) {
    const char *typeDict[] = {
        "int",
        "float",
        "elem",
        "set",
        "N/A",
        "ERROR"
    };

    return typeDict[type];
}

SymbolType getSymbolTypeByName(char *name) {
    if (strcmp("int", name) == 0) {
        return INT_S;
    } else if (strcmp("float", name) == 0) {
        return FLOAT_S;
    } else if (strcmp("elem", name) == 0) {
        return ELEM_S;
    } else if (strcmp("set", name) == 0) {
        return SET_S;
    } else if (strcmp("N/A", name) == 0) {
        return NA_S;
    }

    return ERROR_S;
}