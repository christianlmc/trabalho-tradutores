#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE
#include "ast.h"

typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct FunctionAttributes FunctionAttributes;

typedef enum SymbolType {
    INT_S,
    FLOAT_S,
    ELEM_S,
    SET_S,
    ERROR_S
} SymbolType;

struct SymbolTable {
    Symbol *first;
};

struct Symbol {
    char *id;
    SymbolType type;
    Symbol *next;
    FunctionAttributes *function;
};

struct FunctionAttributes {
    int argsCount;
    Symbol *arguments;
    SymbolTable *table;
};

Symbol *createSymbol(char *id, SymbolType type, Node *node);

/**
 * 
 * @brief Pushes Symbol to the table
 * 
 * @param table 
 * @param symbol 
 */
void pushSymbol(SymbolTable *table, Symbol *symbol);

/**
 * @brief Creates and pushes symbol to table
 * 
 * @param id 
 * @param table 
 * @param type 
 */
void createAndPushSymbol(SymbolTable *table, char *type, char *id, Node *node);

void printSymbolTable(SymbolTable *table, int level);
void freeSymbolTable(SymbolTable *table);
const char *getSymbolTypeName(SymbolType type);
SymbolType getSymbolTypeByName(char *name);
#endif
