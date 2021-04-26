#include "types.h"

const char *getTypeName(TokenType type) {
    const char *typeDict[] = {
        "int",
        "float",
        "elem",
        "set",
        "undefined",
        "N/A",
        "ERROR"
    };

    return typeDict[type];
}

TokenType getTypeByName(char *name) {
    if (strcmp("int", name) == 0) {
        return INT_TYPE;
    } else if (strcmp("float", name) == 0) {
        return FLOAT_TYPE;
    } else if (strcmp("elem", name) == 0) {
        return ELEM_TYPE;
    } else if (strcmp("set", name) == 0) {
        return SET_TYPE;
    } else if (strcmp("undefined", name) == 0) {
        return UNDEF_TYPE;
    } else if (strcmp("N/A", name) == 0) {
        return NA_TYPE;
    }

    return ERROR_TYPE;
}
