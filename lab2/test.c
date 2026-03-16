#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"
#include "test.h"

static int testsPassed = 0;
static int testsFailed = 0;

#define ASSERT(condition) \
if (condition) { testsPassed++; } else { testsFailed++; }

void testBinarySearchFound(void) {
    int keys[] = {2, 4, 6, 8, 10};
    int index = binary_search(keys, 5, 8);

    ASSERT(index == 3);
}

void testBinarySearchNotFound(void) {
    int keys[] = {2, 4, 6, 8, 10};
    int index = binary_search(keys, 5, 7);

    ASSERT(index == -1);
}

void testMakeSortedCase(void) {
    int src_keys[] = {30, 10, 20};
    char *src_values[] = {"thirty", "ten", "twenty"};
    int n = 3;

    int *dst_keys = (int *)malloc(n * sizeof(int));
    char **dst_values = (char **)malloc(n * sizeof(char *));

    if (dst_keys == NULL || dst_values == NULL) {
        ASSERT(0);
        free(dst_keys);
        free(dst_values);
        return;
    }

    make_sorted_case(src_keys, src_values, n, dst_keys, dst_values);

    ASSERT(dst_keys[0] == 10 && dst_keys[1] == 20 && dst_keys[2] == 30 &&
           strcmp(dst_values[0], "ten") == 0 &&
           strcmp(dst_values[1], "twenty") == 0 &&
           strcmp(dst_values[2], "thirty") == 0);

    free_table(dst_keys, dst_values, n);
}

void testMakeReverseCase(void) {
    int src_keys[] = {30, 10, 20};
    char *src_values[] = {"thirty", "ten", "twenty"};
    int n = 3;

    int *dst_keys = (int *)malloc(n * sizeof(int));
    char **dst_values = (char **)malloc(n * sizeof(char *));

    if (dst_keys == NULL || dst_values == NULL) {
        ASSERT(0);
        free(dst_keys);
        free(dst_values);
        return;
    }

    make_reverse_case(src_keys, src_values, n, dst_keys, dst_values);

    ASSERT(dst_keys[0] == 30 && dst_keys[1] == 20 && dst_keys[2] == 10 &&
           strcmp(dst_values[0], "thirty") == 0 &&
           strcmp(dst_values[1], "twenty") == 0 &&
           strcmp(dst_values[2], "ten") == 0);

    free_table(dst_keys, dst_values, n);
}

void testMakeRandomCase(void) {
    int src_keys[] = {4, 1, 3, 2};
    char *src_values[] = {"four", "one", "three", "two"};
    int n = 4;

    int *dst_keys = (int *)malloc(n * sizeof(int));
    char **dst_values = (char **)malloc(n * sizeof(char *));

    if (dst_keys == NULL || dst_values == NULL) {
        ASSERT(0);
        free(dst_keys);
        free(dst_values);
        return;
    }

    make_random_case(src_keys, src_values, n, dst_keys, dst_values);
    shaker_sort(dst_keys, dst_values, n);

    ASSERT(dst_keys[0] == 1 && dst_keys[1] == 2 && dst_keys[2] == 3 && dst_keys[3] == 4 &&
           strcmp(dst_values[0], "one") == 0 &&
           strcmp(dst_values[1], "two") == 0 &&
           strcmp(dst_values[2], "three") == 0 &&
           strcmp(dst_values[3], "four") == 0);

    free_table(dst_keys, dst_values, n);
}

void runAllTests(void) {
    testBinarySearchFound();
    testBinarySearchNotFound();
    testMakeSortedCase();
    testMakeReverseCase();
    testMakeRandomCase();

    printf("Тестов пройдено: %d, провалено: %d, всего: %d\n",
           testsPassed, testsFailed, testsPassed + testsFailed);

    if (testsFailed == 0) {
        printf("Все тесты пройдены!\n");
    } else {
        printf("Есть ошибки в тестах!\n");
    }
}

int main(void) {
    runAllTests();
    return testsFailed > 0 ? 1 : 0;
}
