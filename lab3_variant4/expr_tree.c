#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr_tree.h"
#include "node.h"

typedef struct {
    char *s;
    size_t len;
    size_t cap;
} DynStr;

static bool dyn_append(DynStr *d, const char *chunk)
{
    if (!chunk)
        return true;

    const size_t add = strlen(chunk);
    const size_t need = d->len + add + 1;
    if (need > d->cap) {
        size_t new_cap = (d->cap == 0) ? 64 : d->cap * 2;
        while (new_cap < need)
            new_cap *= 2;
        char *p = realloc(d->s, new_cap);
        if (!p)
            return false;
        d->s = p;
        d->cap = new_cap;
    }

    memcpy(d->s + d->len, chunk, add + 1);
    d->len += add;
    return true;
}

static int gcd_int(int a, int b)
{
    if (a < 0)
        a = -a;
    if (b < 0)
        b = -b;
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return (a == 0) ? 1 : a;
}

static void replace_with_child(Node **node_ptr, bool use_left)
{
    Node *node = *node_ptr;
    Node *child = use_left ? node->left : node->right;
    Node *other = use_left ? node->right : node->left;

    if (other)
        free_tree(other);
    free(node->value);
    free(node);
    *node_ptr = child;
}

static void set_number_node(Node *node, int value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d", value);
    free(node->value);
    node->value = malloc(strlen(buffer) + 1);
    if (!node->value) {
        fprintf(stderr, "Ошибка: не удалось выделить память для числа\n");
        exit(1);
    }
    strcpy(node->value, buffer);
    node->is_number = true;
    node->num_value = value;
    node->is_unary = false;
}

Node *build_tree(char *postfix[], int count)
{
    if (count == 0)
        return NULL;

    Node **stack = malloc((size_t)count * sizeof(Node *));
    if (!stack) {
        fprintf(stderr, "Ошибка: не хватило памяти для стека дерева\n");
        return NULL;
    }

    int top = -1;
    for (int i = 0; i < count; i++) {
        Node *node = create_node(postfix[i]);
        if (!node) {
            free(stack);
            return NULL;
        }

        if (node->is_number || isalpha((unsigned char)postfix[i][0])) {
            stack[++top] = node;
        } else if (postfix[i][0] == '~') {
            node->is_unary = true;
            if (top >= 0)
                node->right = stack[top--];
            stack[++top] = node;
        } else {
            if (top >= 0)
                node->right = stack[top--];
            if (top >= 0)
                node->left = stack[top--];
            stack[++top] = node;
        }
    }

    Node *root = (top >= 0) ? stack[top] : NULL;
    free(stack);
    return root;
}

void print_tree(Node *node, int level)
{
    if (!node)
        return;

    for (int i = 0; i < level; i++)
        printf("    ");

    if (node->is_unary) {
        printf("-\n");
        print_tree(node->right, level + 1);
        return;
    }

    printf("%s\n", node->value);
    print_tree(node->left, level + 1);
    print_tree(node->right, level + 1);
}

static Node *simplify_division_rec(Node *node)
{
    if (!node)
        return NULL;

    node->left = simplify_division_rec(node->left);
    node->right = simplify_division_rec(node->right);

    if (strcmp(node->value, "/") != 0 || !node->left || !node->right)
        return node;

    if (node->right->is_number && node->right->num_value == 0) {
        fprintf(stderr, "Предупреждение: деление на ноль оставлено без изменений\n");
        return node;
    }

    if (node->left->is_number && node->right->is_number) {
        const int a = node->left->num_value;
        const int b = node->right->num_value;
        if (b != 0 && a % b == 0) {
            const int q = a / b;
            free_tree(node->left);
            free_tree(node->right);
            node->left = NULL;
            node->right = NULL;
            set_number_node(node, q);
        }
        return node;
    }

    if (node->right->is_number && node->right->num_value == 1) {
        replace_with_child(&node, true);
        return node;
    }

    if (node->left->is_number && node->right->is_number) {
        return node;
    }

    if (node->left && strcmp(node->left->value, "*") == 0 && node->right->is_number) {
        Node *mul = node->left;
        Node *den = node->right;
        const int d = den->num_value;

        if (mul->left && mul->left->is_number && d != 0) {
            int a = mul->left->num_value;
            int g = gcd_int(a, d);
            if (g > 1) {
                a /= g;
                int new_den = d / g;
                set_number_node(mul->left, a);
                set_number_node(den, new_den);
            }
        } else if (mul->right && mul->right->is_number && d != 0) {
            int a = mul->right->num_value;
            int g = gcd_int(a, d);
            if (g > 1) {
                a /= g;
                int new_den = d / g;
                set_number_node(mul->right, a);
                set_number_node(den, new_den);
            }
        }

        if (den->is_number && den->num_value == 1) {
            Node *promoted = node->left;
            free_tree(node->right);
            free(node->value);
            free(node);
            node = promoted;

            if (strcmp(node->value, "*") == 0) {
                if (node->left && node->left->is_number && node->left->num_value == 1) {
                    replace_with_child(&node, false);
                } else if (node->right && node->right->is_number && node->right->num_value == 1) {
                    replace_with_child(&node, true);
                }
            }
        }
    }

    return node;
}

Node *simplify_division(Node *node)
{
    return simplify_division_rec(node);
}

static bool tree_to_infix_rec(Node *node, DynStr *out)
{
    if (!node)
        return true;

    if (node->is_number ||
        (strlen(node->value) == 1 && isalpha((unsigned char)node->value[0]))) {
        return dyn_append(out, node->value);
    }

    if (node->is_unary || node->value[0] == '~') {
        if (!dyn_append(out, "(-"))
            return false;
        if (!tree_to_infix_rec(node->right, out))
            return false;
        return dyn_append(out, ")");
    }

    if (!dyn_append(out, "("))
        return false;
    if (!tree_to_infix_rec(node->left, out))
        return false;
    if (node->value[0] == '^') {
        if (!dyn_append(out, "^"))
            return false;
    } else {
        if (!dyn_append(out, " ") || !dyn_append(out, node->value) || !dyn_append(out, " "))
            return false;
    }
    if (!tree_to_infix_rec(node->right, out))
        return false;
    return dyn_append(out, ")");
}

char *tree_to_infix(Node *node)
{
    if (!node) {
        char *empty = malloc(1);
        if (empty)
            empty[0] = '\0';
        return empty;
    }

    DynStr d = {0};
    if (!tree_to_infix_rec(node, &d)) {
        free(d.s);
        return NULL;
    }
    if (!d.s) {
        char *empty = malloc(1);
        if (empty)
            empty[0] = '\0';
        return empty;
    }
    return d.s;
}
