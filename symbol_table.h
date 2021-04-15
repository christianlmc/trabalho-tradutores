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

    int line, column;

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
Symbol *createSymbol(char *type, char *id, int line, int column, tinyint isBlock, Node *node);

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

void checkForRedeclaration(Symbol *symbol);

/**
 * @brief Create and pushes a Block to table
 * 
 * @param table 
 * @param type 
 * @param id 
 * @param node 
 */
Symbol *createBlock();

char *convertToBoolean(tinyint boolean);

void debugSymbol(Symbol *symbol);

void printSymbolTable(Symbol *symbol, int level);
void freeSymbolTable(SymbolTable *table);
void freeSymbol(Symbol *symbol);
const char *getSymbolTypeName(SymbolType type);
SymbolType getSymbolTypeByName(char *name);
#endif
