#include "symbol_table.h"

Symbol *createSymbol(char *id, SymbolType type, Node *node) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));

    symbol->id = malloc(strlen(id) + 1);
    strncpy(symbol->id, id, strlen(id) + 1);
    symbol->type = type;

    symbol->next = NULL;

    if (node) {
        FunctionAttributes *function = (FunctionAttributes *)malloc(sizeof(FunctionAttributes));
        SymbolTable *table           = (SymbolTable *)malloc(sizeof(SymbolTable));

        int argsCount    = 0;
        symbol->function = function;
        function->table  = table;

        while (node != NULL) {
            Node *nodeChild = node->child;

            if (nodeChild) {
                char *type = nodeChild->value;
                char *id   = nodeChild->next->value;
                printf("DEBUG NODE: %s, Type: %s, Id: %s\n", node->value, type, id);
                argsCount++;
                createAndPushSymbol(table, type, id, NULL);
            } else {
                printf("DEBUG NODE: %s\n", node->value);
            }
            node = node->next;
        }
        symbol->function->argsCount = argsCount;
    }

    return symbol;
}

void pushSymbol(SymbolTable *table, Symbol *symbol) {
    Symbol *aux = table->first;

    if (aux == NULL) {
        table->first = symbol;
    } else {
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = symbol;
    }
}

void createAndPushSymbol(SymbolTable *table, char *type, char *id, Node *node) {
    Symbol *symbol = createSymbol(id, getSymbolTypeByName(type), node);
    pushSymbol(table, symbol);
}

void printSymbolTable(SymbolTable *table, int level) {
    Symbol *aux = table->first;

    if (level == 0) {
        printf("------------- SYMBOL TABLE -------------\n");
    }
    printf("%-8s%-32s%-5s\n", "TYPE", "SYMBOL", "IS FUNCTION");

    while (aux != NULL) {
        printf("%-8s%-32s%-5s\n", getSymbolTypeName(aux->type), aux->id, aux->function ? "Yes" : "No");
        if (aux->function != NULL) {
            if (aux->function->table->first != NULL) {
                printf("------------- SYMBOL TABLE (%s) -------------\n", aux->id);
                printSymbolTable(aux->function->table, level + 1);
                printf("------------- END SYMBOL TABLE (%s) -------------\n", aux->id);
                printf("%-8s%s\n", "TYPE", "SYMBOL");
            }
        }
        aux = aux->next;
    }
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
    }

    return ERROR_S;
}