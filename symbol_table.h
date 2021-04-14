#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE
#include "ast.h"
#include "colors.h"

typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef unsigned char tinyint;

typedef enum SymbolType {
    INT_S,
    FLOAT_S,
    ELEM_S,
    SET_S,
    NA_S,
    ERROR_S
} SymbolType;

struct SymbolTable {
    Symbol *first;
};

struct Symbol {
    SymbolType type;
    char *id;
    int argsCount;

    tinyint isFunction;
    tinyint isBlock;

    Symbol *next, *prev, *parent, *child;
};

/**
 * @brief Create a Symbol object
 * 
 * @param type 
 * @param id 
 * @param node 
 * @return Symbol* 
 */
Symbol *createSymbol(SymbolType type, char *id, tinyint isBlock, Node *node);

/**
 * @brief Create a global Symbol object
 * 
 * @return Symbol* 
 */
Symbol *createGlobalSymbol();

/**
 * 
 * @brief Pushes child Symbol to another Symbol
 * 
 * @param symbol 
 * @param child 
 */
void pushChildSymbol(Symbol *symbol, Symbol *child);

/**
 * 
 * @brief Pushes next Symbol to another Symbol
 * 
 * @param symbol 
 * @param child 
 */
void pushNextSymbol(Symbol *symbol, Symbol *next);

/**
 * @brief Create and pushes a Block to table
 * 
 * @param table 
 * @param type 
 * @param id 
 * @param node 
 */
Symbol *createBlock();

void debugSymbol(Symbol *symbol);

void printSymbolTable(Symbol *symbol, int level);
void freeSymbolTable(SymbolTable *table);
const char *getSymbolTypeName(SymbolType type);
SymbolType getSymbolTypeByName(char *name);
#endif
