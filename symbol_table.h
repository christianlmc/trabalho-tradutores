#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE
#include "ast.h"
#include "colors.h"

typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef unsigned char tinyint;

struct SymbolTable {
    Symbol *first;
};

struct Symbol {
    TokenType type;
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
void checkForPresence(Symbol *symbol, char *id, int line, int column);
void checkArguments(Symbol *scope, Node *functionNode, Node *args, int line, int column);
TokenType getIdentifierType(Node *identifier, Symbol *scope);
Symbol *findSymbolByName(char *name, Symbol *scope);
Symbol *getLastChildSymbol(Symbol *scope);

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
const char *getTypeName(TokenType type);
TokenType getTypeByName(char *name);
#endif
