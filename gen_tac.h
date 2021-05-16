#include "types.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int availableTacTableVar;
int availableTacVar;
char *tacTable;

char *addCommand(char *code, char *command);
char *formatStr(const char *format, ...);
int getStrLen(char *string);
char *createInstruction(Node *op, Node *left, Node *right);
char *createTableString(char *table, char *string);
char *convertCharToString(char *myChar);
char *createOutputInstruction(Node *op, Node *left);
char *injectSpecialFunctions(char *code);
char *getAddress(Node *node);