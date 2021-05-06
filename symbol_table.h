#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE
#include "ast.h"
#include "colors.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern tinyint hasError;

/**
 * @brief Create a Symbol object
 * 
 * @param type 
 * @param id 
 * @param node 
 * @return Symbol* 
 */
Symbol *createSymbol(char *type, Node *id, tinyint isBlock, Node *args);

Symbol *createSymbolArg(char *type, Node *id, int index);

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
TokenType getIdentifierType(Node *identifier, Symbol *scope);
Symbol *findSymbolByName(char *name, Symbol *scope);
Symbol *getCurrentFunction(Symbol *scope);
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
#endif
