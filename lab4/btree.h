#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>

#define T 2
#define MAX_KEYS (2 * T - 1)
#define MAX_CHILDREN (2 * T)
#define KEY_LEN 7

typedef struct BTreeNode {
    int n;
    char keys[MAX_KEYS][KEY_LEN];
    double values[MAX_KEYS];
    struct BTreeNode *children[MAX_CHILDREN];
    int leaf;
} BTreeNode;

typedef struct {
    BTreeNode *root;
} BTree;

BTree *btree_create(void);
void btree_free(BTree *tree);
int btree_insert(BTree *tree, const char *key, double value);
int btree_delete(BTree *tree, const char *key);
int btree_search(BTree *tree, const char *key, double *value_out);
void btree_print(const BTree *tree, FILE *out);

#endif
