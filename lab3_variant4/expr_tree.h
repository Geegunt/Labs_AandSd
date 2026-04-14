#ifndef EXPR_TREE_H
#define EXPR_TREE_H

#include "node.h"

Node *build_tree(char *postfix[], int count);
void print_tree(Node *node, int level);
Node *simplify_division(Node *node);
char *tree_to_infix(Node *node);

#endif
