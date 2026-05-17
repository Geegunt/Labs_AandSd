#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BTreeNode *node_create(int leaf) {
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!node) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    node->n = 0;
    node->leaf = leaf;
    for (int i = 0; i < MAX_CHILDREN; i++)
        node->children[i] = NULL;
    return node;
}

static void node_free(BTreeNode *node) {
    if (!node) return;
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++)
            node_free(node->children[i]);
    }
    free(node);
}

BTree *btree_create(void) {
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    if (!tree) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    tree->root = node_create(1);
    return tree;
}

void btree_free(BTree *tree) {
    if (!tree) return;
    node_free(tree->root);
    free(tree);
}

static BTreeNode *node_search(BTreeNode *node, const char *key, int *idx_out) {
    int i = 0;
    while (i < node->n && strcmp(key, node->keys[i]) > 0)
        i++;
    if (i < node->n && strcmp(key, node->keys[i]) == 0) {
        *idx_out = i;
        return node;
    }
    if (node->leaf) return NULL;
    return node_search(node->children[i], key, idx_out);
}

int btree_search(BTree *tree, const char *key, double *value_out) {
    int idx;
    BTreeNode *found = node_search(tree->root, key, &idx);
    if (!found) return 0;
    *value_out = found->values[idx];
    return 1;
}

static void split_child(BTreeNode *parent, int i, BTreeNode *child) {
    BTreeNode *new_node = node_create(child->leaf);
    new_node->n = T - 1;

    for (int j = 0; j < T - 1; j++) {
        memcpy(new_node->keys[j], child->keys[j + T], KEY_LEN);
        new_node->values[j] = child->values[j + T];
    }
    if (!child->leaf) {
        for (int j = 0; j < T; j++)
            new_node->children[j] = child->children[j + T];
    }
    child->n = T - 1;

    for (int j = parent->n; j >= i + 1; j--)
        parent->children[j + 1] = parent->children[j];
    parent->children[i + 1] = new_node;

    for (int j = parent->n - 1; j >= i; j--) {
        memcpy(parent->keys[j + 1], parent->keys[j], KEY_LEN);
        parent->values[j + 1] = parent->values[j];
    }
    memcpy(parent->keys[i], child->keys[T - 1], KEY_LEN);
    parent->values[i] = child->values[T - 1];
    parent->n++;
}

static int insert_non_full(BTreeNode *node, const char *key, double value) {
    int i = node->n - 1;
    if (node->leaf) {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) {
            memcpy(node->keys[i + 1], node->keys[i], KEY_LEN);
            node->values[i + 1] = node->values[i];
            i--;
        }
        if (i >= 0 && strcmp(key, node->keys[i]) == 0)
            return 0;
        memcpy(node->keys[i + 1], key, KEY_LEN);
        node->values[i + 1] = value;
        node->n++;
        return 1;
    }
    while (i >= 0 && strcmp(key, node->keys[i]) < 0)
        i--;
    if (i >= 0 && strcmp(key, node->keys[i]) == 0)
        return 0;
    i++;
    if (node->children[i]->n == MAX_KEYS) {
        split_child(node, i, node->children[i]);
        if (strcmp(key, node->keys[i]) > 0)
            i++;
        else if (strcmp(key, node->keys[i]) == 0)
            return 0;
    }
    return insert_non_full(node->children[i], key, value);
}

int btree_insert(BTree *tree, const char *key, double value) {
    BTreeNode *root = tree->root;
    if (root->n == MAX_KEYS) {
        BTreeNode *new_root = node_create(0);
        new_root->children[0] = root;
        split_child(new_root, 0, root);
        tree->root = new_root;
        return insert_non_full(new_root, key, value);
    }
    return insert_non_full(root, key, value);
}

static int find_key_idx(BTreeNode *node, const char *key) {
    int idx = 0;
    while (idx < node->n && strcmp(node->keys[idx], key) < 0)
        idx++;
    return idx;
}

static void remove_from_leaf(BTreeNode *node, int idx) {
    for (int i = idx + 1; i < node->n; i++) {
        memcpy(node->keys[i - 1], node->keys[i], KEY_LEN);
        node->values[i - 1] = node->values[i];
    }
    node->n--;
}

static BTreeNode *get_predecessor_node(BTreeNode *node, int idx) {
    BTreeNode *cur = node->children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->n];
    return cur;
}

static BTreeNode *get_successor_node(BTreeNode *node, int idx) {
    BTreeNode *cur = node->children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];
    return cur;
}

static void merge_children(BTreeNode *node, int idx) {
    BTreeNode *left = node->children[idx];
    BTreeNode *right = node->children[idx + 1];

    memcpy(left->keys[T - 1], node->keys[idx], KEY_LEN);
    left->values[T - 1] = node->values[idx];

    for (int i = 0; i < right->n; i++) {
        memcpy(left->keys[T + i], right->keys[i], KEY_LEN);
        left->values[T + i] = right->values[i];
    }
    if (!left->leaf) {
        for (int i = 0; i <= right->n; i++)
            left->children[T + i] = right->children[i];
    }
    left->n += right->n + 1;

    for (int i = idx + 1; i < node->n; i++) {
        memcpy(node->keys[i - 1], node->keys[i], KEY_LEN);
        node->values[i - 1] = node->values[i];
    }
    for (int i = idx + 2; i <= node->n; i++)
        node->children[i - 1] = node->children[i];
    node->n--;

    free(right);
}

static void borrow_from_prev(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx - 1];

    for (int i = child->n - 1; i >= 0; i--) {
        memcpy(child->keys[i + 1], child->keys[i], KEY_LEN);
        child->values[i + 1] = child->values[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; i--)
            child->children[i + 1] = child->children[i];
    }

    memcpy(child->keys[0], node->keys[idx - 1], KEY_LEN);
    child->values[0] = node->values[idx - 1];
    if (!child->leaf)
        child->children[0] = sibling->children[sibling->n];

    memcpy(node->keys[idx - 1], sibling->keys[sibling->n - 1], KEY_LEN);
    node->values[idx - 1] = sibling->values[sibling->n - 1];

    child->n++;
    sibling->n--;
}

static void borrow_from_next(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];

    memcpy(child->keys[child->n], node->keys[idx], KEY_LEN);
    child->values[child->n] = node->values[idx];
    if (!child->leaf)
        child->children[child->n + 1] = sibling->children[0];

    memcpy(node->keys[idx], sibling->keys[0], KEY_LEN);
    node->values[idx] = sibling->values[0];

    for (int i = 1; i < sibling->n; i++) {
        memcpy(sibling->keys[i - 1], sibling->keys[i], KEY_LEN);
        sibling->values[i - 1] = sibling->values[i];
    }
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; i++)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->n++;
    sibling->n--;
}

static void fill_child(BTreeNode *node, int idx) {
    if (idx != 0 && node->children[idx - 1]->n >= T)
        borrow_from_prev(node, idx);
    else if (idx != node->n && node->children[idx + 1]->n >= T)
        borrow_from_next(node, idx);
    else if (idx != node->n)
        merge_children(node, idx);
    else
        merge_children(node, idx - 1);
}

static int delete_from_node(BTreeNode *node, const char *key);

static void remove_from_internal(BTreeNode *node, int idx) {
    if (node->children[idx]->n >= T) {
        BTreeNode *pred = get_predecessor_node(node, idx);
        memcpy(node->keys[idx], pred->keys[pred->n - 1], KEY_LEN);
        node->values[idx] = pred->values[pred->n - 1];
        delete_from_node(node->children[idx], node->keys[idx]);
    } else if (node->children[idx + 1]->n >= T) {
        BTreeNode *succ = get_successor_node(node, idx);
        memcpy(node->keys[idx], succ->keys[0], KEY_LEN);
        node->values[idx] = succ->values[0];
        delete_from_node(node->children[idx + 1], node->keys[idx]);
    } else {
        char saved[KEY_LEN];
        memcpy(saved, node->keys[idx], KEY_LEN);
        merge_children(node, idx);
        delete_from_node(node->children[idx], saved);
    }
}

static int delete_from_node(BTreeNode *node, const char *key) {
    int idx = find_key_idx(node, key);

    if (idx < node->n && strcmp(node->keys[idx], key) == 0) {
        if (node->leaf)
            remove_from_leaf(node, idx);
        else
            remove_from_internal(node, idx);
        return 1;
    }

    if (node->leaf) return 0;

    int last = (idx == node->n);
    if (node->children[idx]->n < T)
        fill_child(node, idx);

    if (last && idx > node->n)
        return delete_from_node(node->children[idx - 1], key);
    return delete_from_node(node->children[idx], key);
}

int btree_delete(BTree *tree, const char *key) {
    if (!tree->root || tree->root->n == 0) return 0;

    int result = delete_from_node(tree->root, key);

    if (tree->root->n == 0 && !tree->root->leaf) {
        BTreeNode *old_root = tree->root;
        tree->root = tree->root->children[0];
        free(old_root);
    }

    return result;
}

static void print_node(const BTreeNode *node, int level, FILE *out) {
    if (!node) return;
    for (int i = 0; i < level * 4; i++)
        fprintf(out, " ");
    fprintf(out, "[");
    for (int i = 0; i < node->n; i++) {
        if (i > 0) fprintf(out, " | ");
        fprintf(out, "%s:%.6g", node->keys[i], node->values[i]);
    }
    fprintf(out, "]\n");
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++)
            print_node(node->children[i], level + 1, out);
    }
}

void btree_print(const BTree *tree, FILE *out) {
    if (!tree->root || tree->root->n == 0) {
        fprintf(out, "Дерево пусто.\n");
        return;
    }
    print_node(tree->root, 0, out);
}
