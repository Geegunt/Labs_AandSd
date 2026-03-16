#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

char *str_duplicate(const char *src) {
    size_t len = strlen(src);
    char *copy = (char *)malloc((len + 1) * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    strcpy(copy, src);
    return copy;
}

void remove_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swap_str(char **a, char **b) {
    char *temp = *a;
    *a = *b;
    *b = temp;
}

int read_table_from_file(const char *filename, int **keys, char ***values, int *n) {
    FILE *file = fopen(filename, "r");
    int i;
    char *line = NULL;
    size_t line_capacity = 0;
    ssize_t line_length;

    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }

    if (fscanf(file, "%d", n) != 1 || *n <= 0) {
        printf("Ошибка: некорректное количество элементов.\n");
        fclose(file);
        return 0;
    }

    *keys = (int *)malloc((*n) * sizeof(int));
    *values = (char **)malloc((*n) * sizeof(char *));
    if (*keys == NULL || *values == NULL) {
        printf("Ошибка: не удалось выделить память.\n");
        fclose(file);
        free(*keys);
        free(*values);
        return 0;
    }

    fgetc(file);

    for (i = 0; i < *n; i++) {
        if (fscanf(file, "%d", &((*keys)[i])) != 1) {
            printf("Ошибка чтения ключа для элемента %d\n", i);
            free(line);
            fclose(file);
            free_table(*keys, *values, i);
            return 0;
        }

        fgetc(file);

        line_length = getline(&line, &line_capacity, file);
        if (line_length == -1) {
            printf("Ошибка чтения строки для элемента %d\n", i);
            free(line);
            fclose(file);
            free_table(*keys, *values, i);
            return 0;
        }

        remove_newline(line);

        (*values)[i] = str_duplicate(line);
        if ((*values)[i] == NULL) {
            printf("Ошибка: не удалось выделить память под строку.\n");
            free(line);
            fclose(file);
            free_table(*keys, *values, i);
            return 0;
        }
    }

    free(line);
    fclose(file);
    return 1;
}

void print_table(const int *keys, char *const *values, int n, const char *title) {
    int i;

    printf("\n%s\n", title);
    printf("--------------------------------------------------------------\n");
    printf("| %-5s | %-50s |\n", "Key", "Value");
    printf("--------------------------------------------------------------\n");

    for (i = 0; i < n; i++) {
        printf("| %-5d | %-50s |\n", keys[i], values[i]);
    }

    printf("--------------------------------------------------------------\n");
}

void free_table(int *keys, char **values, int n) {
    int i;

    if (values != NULL) {
        for (i = 0; i < n; i++) {
            free(values[i]);
        }
        free(values);
    }

    free(keys);
}

void copy_table(const int *src_keys, char *const *src_values,
                int n, int *dst_keys, char **dst_values) {
    int i;

    for (i = 0; i < n; i++) {
        dst_keys[i] = src_keys[i];
        dst_values[i] = str_duplicate(src_values[i]);
    }
}

void shaker_sort(int *keys, char **values, int n) {
    int left = 0;
    int right = n - 1;
    int i;
    int swapped = 1;
    while (left < right && swapped) {
        swapped = 0;
        for (i = left; i < right; i++) {
            if (keys[i] > keys[i + 1]) {
                swap_int(&keys[i], &keys[i + 1]);
                swap_str(&values[i], &values[i + 1]);
                swapped = 1;
            }
        }
        right--;
        for (i = right; i > left; i--) {
            if (keys[i - 1] > keys[i]) {
                swap_int(&keys[i - 1], &keys[i]);
                swap_str(&values[i - 1], &values[i]);
                swapped = 1;
            }
        }
        left++;
    }
}

int binary_search(const int *keys, int n, int target) {
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (keys[mid] == target) {
            return mid;
        } else if (keys[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

void make_sorted_case(const int *src_keys, char *const *src_values,
                      int n, int *dst_keys, char **dst_values) {
    copy_table(src_keys, src_values, n, dst_keys, dst_values);
    shaker_sort(dst_keys, dst_values, n);
}

void make_reverse_case(const int *src_keys, char *const *src_values,
                       int n, int *dst_keys, char **dst_values) {
    int i;

    copy_table(src_keys, src_values, n, dst_keys, dst_values);
    shaker_sort(dst_keys, dst_values, n);

    for (i = 0; i < n / 2; i++) {
        swap_int(&dst_keys[i], &dst_keys[n - 1 - i]);
        swap_str(&dst_values[i], &dst_values[n - 1 - i]);
    }
}

void make_random_case(const int *src_keys, char *const *src_values,
                      int n, int *dst_keys, char **dst_values) {
    int i;

    copy_table(src_keys, src_values, n, dst_keys, dst_values);

    srand((unsigned int)time(NULL));

    for (i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap_int(&dst_keys[i], &dst_keys[j]);
        swap_str(&dst_values[i], &dst_values[j]);
    }
}
