#include "sort.h"

#include <stdio.h>
#include <stdlib.h>

void run_case(const char *case_name, int *keys, char **values, int n) {
    print_table(keys, values, n, case_name);
    shaker_sort(keys, values, n);
    print_table(keys, values, n, "После сортировки");
}

void search_loop(const int *keys, char *const *values, int n) {
    int target;
    int index;

    printf("\nВведите ключи для двоичного поиска.\n");
    printf("Для завершения введите -1.\n");

    while (1) {
        printf("Ключ: ");
        if (scanf("%d", &target) != 1) {
            printf("Ошибка ввода.\n");
            break;
        }

        if (target == -1) {
            break;
        }

        index = binary_search(keys, n, target);
        if (index != -1) {
            printf("Найден элемент: key = %d, value = %s\n",
                   keys[index], values[index]);
        } else {
            printf("Элемент с ключом %d не найден.\n", target);
        }
    }
}

int main(void) {
    const char *filename = "input.txt";

    int *base_keys = NULL;
    char **base_values = NULL;
    int n = 0;

    int *keys_sorted = NULL;
    char **values_sorted = NULL;

    int *keys_reverse = NULL;
    char **values_reverse = NULL;

    int *keys_random = NULL;
    char **values_random = NULL;

    if (!read_table_from_file(filename, &base_keys, &base_values, &n)) {
        return 1;
    }

    if (n < 12) {
        printf("Ошибка: по варианту должно быть не менее 12 элементов.\n");
        free_table(base_keys, base_values, n);
        return 1;
    }

    keys_sorted = (int *)malloc(n * sizeof(int));
    values_sorted = (char **)malloc(n * sizeof(char *));
    keys_reverse = (int *)malloc(n * sizeof(int));
    values_reverse = (char **)malloc(n * sizeof(char *));
    keys_random = (int *)malloc(n * sizeof(int));
    values_random = (char **)malloc(n * sizeof(char *));

    if (keys_sorted == NULL || values_sorted == NULL ||
        keys_reverse == NULL || values_reverse == NULL ||
        keys_random == NULL || values_random == NULL) {
        printf("Ошибка: не удалось выделить память.\n");

        free(base_keys);
        free(base_values);
        free(keys_sorted);
        free(values_sorted);
        free(keys_reverse);
        free(values_reverse);
        free(keys_random);
        free(values_random);

        return 1;
    }

    make_sorted_case(base_keys, base_values, n, keys_sorted, values_sorted);
    make_reverse_case(base_keys, base_values, n, keys_reverse, values_reverse);
    make_random_case(base_keys, base_values, n, keys_random, values_random);

    printf("Случай 1: таблица уже упорядочена\n");
    run_case("Исходная таблица", keys_sorted, values_sorted, n);

    printf("\nСлучай 2: таблица в обратном порядке\n");
    run_case("Исходная таблица", keys_reverse, values_reverse, n);

    printf("\nСлучай 3: таблица не упорядочена\n");
    run_case("Исходная таблица", keys_random, values_random, n);

    search_loop(keys_random, values_random, n);

    free_table(base_keys, base_values, n);
    free_table(keys_sorted, values_sorted, n);
    free_table(keys_reverse, values_reverse, n);
    free_table(keys_random, values_random, n);

    return 0;
}
