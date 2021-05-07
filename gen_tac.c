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
        } else if (strcmp(op->value, "write()") == 0) {
            instruction = formatStr("print %s", leftAddr);
        } else if (strcmp(op->value, "writeln()") == 0) {
            instruction = formatStr("println %s", leftAddr);
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
            char *seqOp = formatStr("seq %s, %s, %s", opAddr, rightAddr, leftAddr);
            char *notOp = formatStr("not %s, %s", opAddr, opAddr);
            instruction = addCommand(seqOp, notOp);
            free(notOp);
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

char *createOutputInstruction(Node *op, Node *literal) {
    char *instruction = "ERROR\n";
    char *opAddr      = getAddress(op);
    char *literalAddr = getAddress(literal);
    // Output
    if (strcmp(op->value, "write()") == 0) {
        char *movOp   = formatStr("mov %s, &str%d", literalAddr, availableTacTableVar);
        char *paramOp = formatStr("param %s", literalAddr);
        char *callOp  = "call write, 1";
        instruction   = addCommand(movOp, paramOp);
        instruction   = addCommand(instruction, callOp);

        free(paramOp);
    } else if (strcmp(op->value, "writeln()") == 0) {
        char *movOp   = formatStr("mov %s, &str%d", literalAddr, availableTacTableVar);
        char *paramOp = formatStr("param %s", literalAddr);
        char *callOp  = "call write, 1\nprintln";
        instruction   = addCommand(movOp, paramOp);
        instruction   = addCommand(instruction, callOp);

        free(paramOp);
    }

    free(opAddr);
    free(literalAddr);

    return instruction;
}

char *createTableString(char *table, char *string) {
    char *strDeclaration = formatStr("char str%d[] = %s", availableTacTableVar, string);
    availableTacTableVar++;
    char *entry = addCommand(table, strDeclaration);
    free(strDeclaration);

    return entry;
}

char *injectSpecialFunctions(char *code) {
    // write and writeln
    code = addCommand(code, "write:");
    code = addCommand(code, "    mov $0, 0");
    code = addCommand(code, "    __write_loop:");
    code = addCommand(code, "    mov $1, #0");
    code = addCommand(code, "    mov $1, $1[$0]");
    code = addCommand(code, "    seq $2, $1, '\\0'");
    code = addCommand(code, "    brnz __write_loop_end, $2");
    code = addCommand(code, "    print $1");
    code = addCommand(code, "    add $0, $0, 1");
    code = addCommand(code, "    jump __write_loop");
    code = addCommand(code, "    __write_loop_end:");
    code = addCommand(code, "    return\n");

    return code;
}

char *getAddress(Node *node) {
    return formatStr("%s%d", node->tacType, node->tacSymbol);
}

char *convertCharToString(char *myChar) {
    for (int i = 0; myChar[i] != '\0'; i++) {
        if (myChar[i] == '\'') {
            myChar[i] = '"';
        }
    }
    return myChar;
}