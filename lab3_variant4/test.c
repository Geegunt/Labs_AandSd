#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr_tree.h"
#include "parser.h"

static void free_postfix(char **tokens, int count)
{
    if (!tokens)
        return;
    for (int i = 0; i < count; i++)
        free(tokens[i]);
    free(tokens);
}

static Node *build_from_infix(const char *expr)
{
    char **post = NULL;
    int n = infix_to_postfix(expr, &post);
    assert(n > 0);
    Node *root = build_tree(post, n);
    free_postfix(post, n);
    return root;
}

static void assert_result(const char *expr, const char *expected)
{
    Node *root = build_from_infix(expr);
    root = simplify_division(root);
    char *result = tree_to_infix(root);
    assert(result != NULL);
    assert(strcmp(result, expected) == 0);
    free(result);
    free_tree(root);
}

static void test_variant_examples(void)
{
    assert_result("4*a/2", "(2 * a)");
    assert_result("(4*a)/2", "(2 * a)");
    assert_result("12*x/3", "(4 * x)");
    printf("Тест 1 пройден\n");
}

static void test_numeric_division(void)
{
    assert_result("8/2", "4");
    assert_result("18/6", "3");
    assert_result("7/2", "(7 / 2)");
    printf("Тест 2 пройден\n");
}

static void test_no_change_cases(void)
{
    assert_result("a/2", "(a / 2)");
    assert_result("(a*b)/4", "((a * b) / 4)");
    assert_result("x/0", "(x / 0)");
    printf("Тест 3 пройден\n");
}

static void test_remove_unit_denominator(void)
{
    assert_result("4*a/4", "a");
    assert_result("5*x/1", "(5 * x)");
    printf("Тест 4 пройден\n");
}

int main(void)
{
    printf("Запуск тестов для варианта 4\n\n");
    test_variant_examples();
    test_numeric_division();
    test_no_change_cases();
    test_remove_unit_denominator();
    printf("\nВсе тесты пройдены успешно\n");
    return 0;
}
