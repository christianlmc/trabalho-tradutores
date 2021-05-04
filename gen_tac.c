#include "gen_tac.h"
#include <stdio.h>
#include <stdlib.h>

char *addCommand(char *code, char *command) {
    int requiredSize = 0;
    requiredSize     = getStrLen(code) + getStrLen(command) + strlen("\n") + 1;

    char *buffer = malloc(requiredSize);
    snprintf(buffer, requiredSize, "%s\n%s", code, command);
    return buffer;
}

int getStrLen(char *string) {
    if (string) {
        return strlen(string);
    }
    return 0;
}

char *formatStr(const char *format, ...) {
    va_list list;

    va_start(list, format);
    char *buffer = malloc(vsnprintf(NULL, 0, format, list) + 1);

    va_start(list, format);
    vsprintf(buffer, format, list);

    return buffer;
}