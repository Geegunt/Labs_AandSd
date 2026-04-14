#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef struct Node {
    char *value;
    struct Node *left;
    struct Node *right;
    bool is_unary;
    bool is_number;
    int num_value;
} Node;

Node *create_node(const char val[]);
void free_tree(Node *node);

#endif
