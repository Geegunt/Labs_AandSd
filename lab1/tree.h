#ifndef TREES_H
#define TREES_H

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int value);
void freeTree(Node* root);
Node* insert(Node* root, int value);
Node* deleteNode(Node* root, int value);
void printTree(Node* root, int level);
void taskVariant4(Node* root);
Node* findMin(Node* root);

#endif