#include <stdio.h>
#include <stdlib.h>

typedef struct Symbol Symbol;

typedef enum SymbolType {
    INT_S,
    FLOAT_S,
    ELEM_S,
    SET_S,
    FUNCTION_S
} SymbolType;

typedef struct SymbolTable {
    Symbol *first;
} SymbolTable;

struct Symbol {
    Symbol *next;
    char *value;
    SymbolType type;
};

Symbol *createSymbol(char *value, SymbolType type);
void pushSymbol(SymbolTable *table, Symbol *symbol);
void printSymbolTable(SymbolTable *table);
void freeSymbolTable(SymbolTable *table);
const char *getSymbolTypeName(SymbolType type);
SymbolType getSymbolTypeByName(char *name);