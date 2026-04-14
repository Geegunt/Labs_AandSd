#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

Node *create_node(const char val[])
{
    if (!val)
        return NULL;
    Node *node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Ошибка: не удалось выделить память для узла\n");
        exit(1);
    }
    node->value = malloc(strlen(val) + 1);
    if (!node->value) {
        fprintf(stderr, "Ошибка: не удалось выделить память для строки\n");
        free(node);
        exit(1);
    }
    strcpy(node->value, val);
    node->left = NULL;
    node->right = NULL;
    node->is_unary = false;
    node->is_number = false;
    node->num_value = 0;
    char *endptr = NULL;
    errno = 0;
    long parsed = strtol(val, &endptr, 10);
    if (errno == 0 && endptr != val && *endptr == '\0' &&
        parsed >= INT_MIN && parsed <= INT_MAX) {
        node->is_number = true;
        node->num_value = (int)parsed;
    }

    return node;
}

void free_tree(Node *node)
{
    if (!node)
        return;

    free_tree(node->left);
    free_tree(node->right);
    free(node->value);
    free(node);
}
