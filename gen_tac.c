#include "gen_tac.h"

char *addCommand(char *code, char *command) {
    int requiredSize = 0;
    requiredSize     = getStrLen(code) + getStrLen(command) + strlen("\n") + 1;

    char *buffer = malloc(requiredSize);
    snprintf(buffer, requiredSize, "%s\n%s", code, command);
    if (code != NULL) {
        free(code);
    }

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

char *createInstruction(Node *op, Node *left, Node *right) {
    char *instruction = "ERROR\n";
    if (right == NULL) {
        char *opAddr   = getAddress(op);
        char *leftAddr = getAddress(left);

        if (strcmp(op->value, "!") == 0) {
            instruction = formatStr("not %s, %s", opAddr, leftAddr);
        }

        free(opAddr);
        free(leftAddr);
    } else {
        char *opAddr    = getAddress(op);
        char *leftAddr  = getAddress(left);
        char *rightAddr = getAddress(right);

        // Relational
        if (strcmp(op->value, "<=") == 0) {
            instruction = formatStr("sleq %s, %s, %s", opAddr, leftAddr, rightAddr);
        } else if (strcmp(op->value, ">=") == 0) {
            instruction = formatStr("sleq %s, %s, %s", opAddr, rightAddr, leftAddr);
        } else if (strcmp(op->value, "==") == 0) {
            instruction = formatStr("seq %s, %s, %s", opAddr, leftAddr, rightAddr);
        } else if (strcmp(op->value, "!=") == 0) {
            char *seq   = formatStr("seq %s, %s, %s", opAddr, rightAddr, leftAddr);
            char *notC  = formatStr("not %s, %s", opAddr, opAddr);
            instruction = addCommand(seq, notC);
            free(notC);
        } else if (strcmp(op->value, "<") == 0) {
            instruction = formatStr("slt %s, %s, %s", opAddr, leftAddr, rightAddr);
        } else if (strcmp(op->value, ">") == 0) {
            instruction = formatStr("slt %s, %s, %s", opAddr, rightAddr, leftAddr);
        }

        // Aritmetic
        if (strcmp(op->value, "+") == 0) {
            instruction = formatStr("add %s, %s, %s", opAddr, leftAddr, rightAddr);
        } else if (strcmp(op->value, "-") == 0) {
            instruction = formatStr("sub %s, %s, %s", opAddr, rightAddr, leftAddr);
        } else if (strcmp(op->value, "*") == 0) {
            instruction = formatStr("mul %s, %s, %s", opAddr, rightAddr, leftAddr);
        } else if (strcmp(op->value, "/") == 0) {
            instruction = formatStr("div %s, %s, %s", opAddr, rightAddr, leftAddr);
        } else if (strcmp(op->value, "=") == 0) {
            instruction = formatStr("mov %s, %s", leftAddr, rightAddr);
        } else if (strcmp(op->value, "sign") == 0) {
            if (strcmp(left->value, "+") == 0) {
                instruction = formatStr("mov %s, %s", opAddr, rightAddr);
            } else if (strcmp(left->value, "-") == 0) {
                instruction = formatStr("minus %s, %s", opAddr, rightAddr);
            }
        }

        // Logic
        if (strcmp(op->value, "||") == 0) {
            instruction = formatStr("or %s, %s, %s", opAddr, leftAddr, rightAddr);
        } else if (strcmp(op->value, "&&") == 0) {
            instruction = formatStr("and %s, %s, %s", opAddr, leftAddr, rightAddr);
        }
        free(opAddr);
        free(leftAddr);
        free(rightAddr);
    }

    return instruction;
}

char *getAddress(Node *node) {
    return formatStr("%s%d", node->tacType, node->tacSymbol);
}
// Exemplo: $0 op $1 com $r resultado
// "<=" sleq $r, $0, $1
// ">=" sleq $r, $1, $0
// "==" seq  $r, $0, $1
// "!=" seq  $r, $0, $1
//      not  $r, $r
// '<'  slt  $r, $0, $1
// '>'  slt  $r, $1, $0
// '!'  not  $r, $0
// '+'  add  $r, $0, $1
// '-'  sub  $r, $0, $1
// '*'  mul  $r, $0, $1
// '/'  div  $r, $0, $1
// '||' or   $r, $0, $1
// '&&' and  $r, $0, $1