#include <stdio.h>
#include <stdlib.h>

#include "expr_tree.h"
#include "parser.h"

static const size_t LINE_BUF_INITIAL = 256;

static int read_line_stdin_realloc(char **line_out, size_t *len_out)
{
    size_t len = 0;
    size_t cap = LINE_BUF_INITIAL;
    char *buf = malloc(cap);
    if (!buf) {
        fprintf(stderr, "Ошибка: не хватило памяти для строки ввода\n");
        *line_out = NULL;
        *len_out = 0;
        return -1;
    }

    int c;
    for (;;) {
        c = fgetc(stdin);
        if (c == EOF || c == '\n' || c == '\r')
            break;
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) {
                free(buf);
                fprintf(stderr, "Ошибка: не хватило памяти для строки ввода\n");
                *line_out = NULL;
                *len_out = 0;
                return -1;
            }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }

    if (c == EOF && len == 0) {
        free(buf);
        *line_out = NULL;
        *len_out = 0;
        return -1;
    }

    buf[len] = '\0';
    *line_out = buf;
    *len_out = len;
    return 0;
}

static void free_postfix(char **postfix, int count)
{
    if (!postfix)
        return;
    for (int i = 0; i < count; i++)
        free(postfix[i]);
    free(postfix);
}

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    char **postfix = NULL;

    printf("Лабораторная работа 3, вариант 4\n");
    printf("Преобразование: упрощение выражений, выполняя деление.\n");
    printf("Введите выражение: ");
    fflush(stdout);

    if (read_line_stdin_realloc(&line, &len) != 0) {
        fprintf(stderr, "Ошибка ввода\n");
        return 1;
    }

    if (len == 0) {
        free(line);
        printf("Пустое выражение\n");
        return 0;
    }

    printf("\nИсходное выражение: %s\n", line);

    int count = infix_to_postfix(line, &postfix);
    if (count < 0) {
        free(line);
        printf("Разбор выражения не выполнен\n");
        return 1;
    }
    free(line);

    printf("ОПН:");
    for (int i = 0; i < count; i++)
        printf(" %s", postfix[i]);
    printf("\n");

    Node *root = build_tree(postfix, count);
    if (!root) {
        fprintf(stderr, "Ошибка построения дерева\n");
        free_postfix(postfix, count);
        return 1;
    }

    printf("\nДерево до преобразования:\n");
    print_tree(root, 0);

    root = simplify_division(root);

    printf("\nДерево после преобразования:\n");
    print_tree(root, 0);

    char *result = tree_to_infix(root);
    if (!result) {
        fprintf(stderr, "Ошибка: не удалось восстановить выражение\n");
        free_tree(root);
        free_postfix(postfix, count);
        return 1;
    }

    printf("\nИтоговое выражение: %s\n", result);

    free(result);
    free_tree(root);
    free_postfix(postfix, count);
    return 0;
}
