#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "btree.h"

/* Тест 1: вставка и поиск — найденное значение совпадает */
static void test_insert_and_search(void)
{
    BTree *tree = btree_create();

    btree_insert(tree, "apple", 1.5);
    btree_insert(tree, "banana", 2.7);
    btree_insert(tree, "cherry", 3.9);

    double val = 0.0;
    assert(btree_search(tree, "banana", &val) == 1);
    assert(val == 2.7);

    btree_free(tree);
    printf("Тест 1 пройден: вставка и поиск\n");
}

/* Тест 2: поиск несуществующего ключа возвращает 0 */
static void test_search_not_found(void)
{
    BTree *tree = btree_create();

    btree_insert(tree, "dog", 10.0);
    btree_insert(tree, "cat", 20.0);

    double val = 0.0;
    assert(btree_search(tree, "fox", &val) == 0);

    btree_free(tree);
    printf("Тест 2 пройден: поиск несуществующего ключа\n");
}

/* Тест 3: удаление — после удаления ключ не находится */
static void test_delete(void)
{
    BTree *tree = btree_create();

    btree_insert(tree, "alpha", 1.0);
    btree_insert(tree, "beta",  2.0);
    btree_insert(tree, "gamma", 3.0);

    assert(btree_delete(tree, "beta") == 1);

    double val = 0.0;
    assert(btree_search(tree, "beta", &val) == 0);

    /* остальные ключи остались на месте */
    assert(btree_search(tree, "alpha", &val) == 1 && val == 1.0);
    assert(btree_search(tree, "gamma", &val) == 1 && val == 3.0);

    btree_free(tree);
    printf("Тест 3 пройден: удаление узла\n");
}

/* Тест 4: вставка дубликата запрещена */
static void test_duplicate_insert(void)
{
    BTree *tree = btree_create();

    assert(btree_insert(tree, "key", 1.0) == 1);  /* первая вставка OK */
    assert(btree_insert(tree, "key", 2.0) == 0);  /* дубликат отклонён */

    /* значение не перезаписано */
    double val = 0.0;
    btree_search(tree, "key", &val);
    assert(val == 1.0);

    btree_free(tree);
    printf("Тест 4 пройден: дубликат не вставляется\n");
}

/* Тест 5: вставка 10+ узлов, дерево сплиттится и все ключи находятся */
static void test_split_and_search_many(void)
{
    BTree *tree = btree_create();

    const char *keys[] = {
        "aa", "ab", "ac", "ba", "bb",
        "bc", "ca", "cb", "cc", "da"
    };
    const double vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = 10;

    for (int i = 0; i < n; i++)
        btree_insert(tree, keys[i], vals[i]);

    double val = 0.0;
    for (int i = 0; i < n; i++) {
        assert(btree_search(tree, keys[i], &val) == 1);
        assert(val == vals[i]);
    }

    btree_free(tree);
    printf("Тест 5 пройден: сплит при массовой вставке\n");
}

int main(void)
{
    printf("Запуск тестов для lab4 (B-дерево порядка 2)\n\n");

    test_insert_and_search();
    test_search_not_found();
    test_delete();
    test_duplicate_insert();
    test_split_and_search_many();

    printf("\nВсе тесты пройдены!\n");
    return 0;
}
