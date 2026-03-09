#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "test.h"

static int testsPassed = 0;
static int testsFailed = 0;

#define ASSERT(condition) \
if (condition) { testsPassed++; } else { testsFailed++; }

void testInsert() {
    Node* root = NULL;
    root = insert(root, 10);
    root = insert(root, 5);
    root = insert(root, 15);

    ASSERT(root != NULL && root->data == 10);

    freeTree(root);
}

void testFindMin() {
    Node* root = NULL;
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);

    Node* min = findMin(root);

    ASSERT(min != NULL && min->data == 20);

    freeTree(root);
}

void testDelete() {
    Node* root = NULL;
    root = insert(root, 10);
    root = insert(root, 5);

    root = deleteNode(root, 5);

    ASSERT(root->left == NULL);

    freeTree(root);
}

void testTreeStructure() {
    Node* root = NULL;
    root = insert(root, 8);
    root = insert(root, 3);
    root = insert(root, 10);

    ASSERT(root->left->data < root->data && root->right->data > root->data);

    freeTree(root);
}

void testDeleteRoot() {
    Node* root = NULL;
    root = insert(root, 10);
    root = insert(root, 5);
    root = insert(root, 15);
    root = insert(root, 12);
    root = insert(root, 20);

    root = deleteNode(root, 10);

    ASSERT(root->data == 12);

    freeTree(root);
}

void runAllTests() {
    testInsert();
    testFindMin();
    testDelete();
    testTreeStructure();
    testDeleteRoot();

    int total = testsPassed + testsFailed;

    printf("Тестов пройдено: %d, провалено: %d, всего: %d\n",
           testsPassed, testsFailed, total);

    if (testsFailed == 0) {
        printf("Все тесты пройдены!\n");
    } else {
        printf("Есть ошибки в тестах!\n");
    }
}

int main() {
    runAllTests();
    return testsFailed > 0 ? 1 : 0;
}