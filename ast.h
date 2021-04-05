#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node {
    Node *child;
    Node *next;
    char *value;
};

Node *createNode(char *value);
void freeTree(Node *node);
void printTree(Node *node, int level);