#include "types.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *addCommand(char *code, char *command);
char *formatStr(const char *format, ...);
int getStrLen(char *string);
char *createInstruction(Node *op, Node *left, Node *right);
char *getAddress(Node *node);