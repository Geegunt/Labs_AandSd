#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "tree.h"

Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Ошибка выделения памяти!\n");
        exit(1);
    }
    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

Node* insert(Node* root, int value) {
    if (root == NULL) {
        return createNode(value);
    }
    if (value < root->data) {
        root->left = insert(root->left, value);
    } else if (value > root->data) {
        root->right = insert(root->right, value);
    }
    return root;
}

Node* findMin(Node* root) {
    Node* current = root;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

Node* deleteNode(Node* root, int value) {
    if (root == NULL) return root;
    if (value < root->data) {
        root->left = deleteNode(root->left, value);
    } else if (value > root->data) {
        root->right = deleteNode(root->right, value);
    } else {
        if (root->left == NULL) {
            Node* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node* temp = root->left;
            free(root);
            return temp;
        }
        Node* temp = findMin(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data);
    }
    return root;
}

void printTree(Node* root, int level) {
    if (root == NULL) return;
    printTree(root->right, level + 1);
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d\n", root->data);
    printTree(root->left, level + 1);
}

void findMinDepthLeafRec(Node* root, int depth, int* minDepth, int* result) {
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL) {
        if (depth < *minDepth) {
            *minDepth = depth;
            *result = root->data;
        }
        return;
    }
    findMinDepthLeafRec(root->left, depth + 1, minDepth, result);
    findMinDepthLeafRec(root->right, depth + 1, minDepth, result);
}

void taskVariant4(Node* root) {
    if (root == NULL) {
        printf("Дерево пусто.\n");
        return;
    }
    int minDepth = INT_MAX;
    int resultValue = -1;
    findMinDepthLeafRec(root, 0, &minDepth, &resultValue);
    if (minDepth != INT_MAX) {
        printf("Значение листа с минимальной глубиной (%d): %d\n", minDepth, resultValue);
    }
}