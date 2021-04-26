#include "symbol_table.h"

Symbol *createSymbol(char *type, Node *id, tinyint isBlock, Node *args) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    int argsCount  = 0;

    symbol->isFunction = args != NULL;
    symbol->type       = getTypeByName(type);
    symbol->id         = strdup(id->value);
    symbol->isBlock    = isBlock;
    symbol->line       = id->line;
    symbol->column     = id->column;
    symbol->next       = NULL;
    symbol->prev       = NULL;
    symbol->parent     = NULL;
    symbol->child      = NULL;

    while (args != NULL) {
        Node *nodeChild = args->child;

        if (nodeChild) {
            char *type = nodeChild->value;
            Node *id2  = nodeChild->next;
            argsCount++;
            pushChildSymbol(symbol, createSymbol(type, id2, 0, NULL));
        }
        args = args->next;
    }

    symbol->argsCount = argsCount;

    return symbol;
}

void checkForRedeclaration(Symbol *symbol) {
    Symbol *aux = symbol;

    while (aux != NULL) {
        aux = aux->prev;

        if (aux != NULL && aux->type != NA_TYPE && strcmp(aux->id, symbol->id) == 0) {
            printf(BOLDRED "Error on %d:%d" RESET ": Redeclaration of '%s' (was previously declared in %d:%d)\n", symbol->line, symbol->column, symbol->id, aux->line, aux->column);
        }
    }
}

TokenType getIdentifierType(Node *identifier, Symbol *scope) {
    Symbol *identifierSymbol = findSymbolByName(identifier->value, scope);

    if (identifierSymbol) {
        return identifierSymbol->type;
    } else {
        return ERROR_TYPE;
    }
}

Symbol *findSymbolByName(char *name, Symbol *scope) {
    Symbol *symbol = getLastChildSymbol(scope);

    while (symbol != NULL) {
        if (strcmp(name, symbol->id) == 0) {
            return symbol;
        }

        if (symbol->prev != NULL) {
            symbol = symbol->prev;
        } else if (symbol->parent != NULL) {
            symbol = symbol->parent;
        } else {
            break;
        }
    }

    return NULL;
}

Symbol *getCurrentFunction(Symbol *scope) {
    while (scope != NULL) {
        if (scope->isFunction) {
            break;
        } else {
            scope = scope->parent;
        }
    }

    return scope;
}

Symbol *getLastChildSymbol(Symbol *scope) {
    Symbol *symbol = scope->child;
    if (symbol) {
        while (symbol->next != NULL) {
            symbol = symbol->next;
        }
    }

    return symbol ? symbol : scope;
}

Symbol *createGlobalSymbol() {
    Node *auxNode  = createNodeFromString("N/A");
    Symbol *symbol = createSymbol("N/A", auxNode, 0, NULL);
    freeTree(auxNode);
    return symbol;
}

void pushChildSymbol(Symbol *symbol, Symbol *child) {
    // printf("Pushing %s to be child of %s\n", child->id, symbol->id);
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

    checkForRedeclaration(next);
}

Symbol *createBlock() {
    Node *auxNode  = createNodeFromString("N/A");
    Symbol *symbol = createSymbol("N/A", auxNode, 1, NULL);
    freeTree(auxNode);
    return symbol;
}

void debugSymbol(Symbol *symbol) {
    if (symbol) {
        printf(RED "DEBUG %s:\n", symbol->id);
        printf("id:         %s\n", symbol->id);
        printf("type:       %s\n", getTypeName(symbol->type));
        printf("isFunction: %d\n", symbol->isFunction);
        printf("argsCount:  %d\n", symbol->argsCount);
        printf("isBlock:    %d\n", symbol->isBlock);
        printf("next:       %d\n", !!symbol->next);
        printf("prev:       %d\n", !!symbol->prev);
        printf("parent:     %d\n", !!symbol->parent);
        printf("child:      %d\n", !!symbol->child);
        printf(RESET);
    } else {
        printf(RED "DEBUG SYMBOL GOT NULL\n" RESET);
    }
}

void printSymbolTable(Symbol *symbol, int level) {
    if (level == 0) {
        printf("------------- SYMBOL TABLE -------------\n");
    }
    if (symbol->type != NA_TYPE) {
        printf("%-8s%-32s%-12s%-8s%-12s%-12s\n", "TYPE", "SYMBOL", "FUNCTION", "BLOCK", "ARGS COUNT", "POSITION");
    }

    while (symbol != NULL) {
        if (symbol->type != NA_TYPE) {
            printf("%-8s%-32s%-12s%-8s%-12d%d:%d\n",
                getTypeName(symbol->type),
                symbol->id,
                convertToBoolean(symbol->isFunction),
                convertToBoolean(symbol->isBlock),
                symbol->argsCount,
                symbol->line, symbol->column);
        }
        if (symbol->child != NULL) {
            printf("------------- SYMBOL TABLE (%s) -------------\n", symbol->type == NA_TYPE ? "N/A" : symbol->id);
            printSymbolTable(symbol->child, level + 1);
            printf("------------- END SYMBOL TABLE (%s) -------------\n", symbol->type == NA_TYPE ? "N/A" : symbol->id);
        }
        symbol = symbol->next;
    }
}

char *convertToBoolean(tinyint boolean) {
    return boolean ? "Yes" : "No";
}

void freeSymbolTable(SymbolTable *table) {
    freeSymbol(table->first);
    free(table);
}

void freeSymbol(Symbol *symbol) {
    Symbol *aux = symbol->next;
    while (symbol != NULL) {
        aux = symbol->next;
        if (symbol->child) {
            freeSymbol(symbol->child);
        }
        free(symbol->id);
        free(symbol);
        symbol = aux;
    }
}