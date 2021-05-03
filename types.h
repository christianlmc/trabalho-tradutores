#ifndef TYPES
#define TYPES
#include <string.h>

typedef unsigned char tinyint;
typedef struct SymbolTable SymbolTable;
typedef struct Symbol Symbol;
typedef struct Node Node;
typedef struct Token Token;

tinyint hasError;

typedef enum TokenType {
    INT_TYPE,
    FLOAT_TYPE,
    ELEM_TYPE,
    SET_TYPE,
    UNDEF_TYPE,
    NA_TYPE,
    ERROR_TYPE
} TokenType;

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

struct Node {
    TokenType type;
    Node *child;
    Node *next;
    char *value;
    int line;
    int column;
};

struct Token {
    int line;
    int column;
    char *value;
};

const char *getTypeName(TokenType type);
TokenType getTypeByName(char *name);
#endif