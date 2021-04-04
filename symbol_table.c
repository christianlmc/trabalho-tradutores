#include "symbol_table.h"
#include <string.h>

Symbol *createSymbol(char *value, SymbolType type) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));

    symbol->value = malloc(strlen(value) + 1);
    strncpy(symbol->value, value, strlen(value) + 1);
    symbol->type = type;

    symbol->next = NULL;

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

void printSymbolTable(SymbolTable *table) {
    Symbol *aux = table->first;

    printf("------------- SYMBOL TABLE -------------\n");
    printf("%-30s%s\n", "SYMBOL", "TYPE");
    while (aux != NULL) {
        printf("%-30s%s\n", aux->value, getSymbolTypeName(aux->type));
        aux = aux->next;
    }
}

void freeSymbolTable(SymbolTable *table) {
    Symbol *aux = table->first;
    Symbol *aux2;

    while (aux != NULL) {
        aux2 = aux->next;
        free(aux->value);
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
        "function"
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
    } else {
        return FUNCTION_S;
    }
}