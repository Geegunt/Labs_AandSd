#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "btree.h"

static int validate_key(const char *key) {
    int len = (int)strlen(key);
    if (len == 0 || len > 6) return 0;
    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)key[i])) return 0;
    }
    return 1;
}

static void process_command(BTree *tree, int op, const char *key, double value, FILE *out) {
    switch (op) {
        case 1:
            if (!validate_key(key)) {
                fprintf(out, "Ошибка: недопустимый ключ '%s'\n", key);
                break;
            }
            if (btree_insert(tree, key, value))
                fprintf(out, "Добавлен: %s = %.6g\n", key, value);
            else
                fprintf(out, "Ошибка: ключ '%s' уже существует\n", key);
            break;
        case 2:
            if (!validate_key(key)) {
                fprintf(out, "Ошибка: недопустимый ключ '%s'\n", key);
                break;
            }
            if (btree_delete(tree, key))
                fprintf(out, "Удалён: %s\n", key);
            else
                fprintf(out, "Ошибка: ключ '%s' не найден\n", key);
            break;
        case 3:
            btree_print(tree, out);
            break;
        case 4: {
            if (!validate_key(key)) {
                fprintf(out, "Ошибка: недопустимый ключ '%s'\n", key);
                break;
            }
            double val;
            if (btree_search(tree, key, &val))
                fprintf(out, "%s = %.6g\n", key, val);
            else
                fprintf(out, "Ключ '%s' не найден\n", key);
            break;
        }
        default:
            fprintf(out, "Неизвестная операция: %d\n", op);
    }
}

static void run_file_mode(const char *in_path, const char *out_path) {
    FILE *fin = fopen(in_path, "r");
    if (!fin) {
        fprintf(stderr, "Ошибка: не удалось открыть файл '%s'\n", in_path);
        return;
    }
    FILE *fout = out_path ? fopen(out_path, "w") : stdout;
    if (!fout) {
        fprintf(stderr, "Ошибка: не удалось открыть файл '%s' для записи\n", out_path);
        fclose(fin);
        return;
    }

    BTree *tree = btree_create();
    char line[256];

    while (fgets(line, sizeof(line), fin)) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        fprintf(fout, "%s\n", line);

        int op;
        char key[KEY_LEN];
        double value = 0.0;

        int parsed = sscanf(line, "%d %6s %lf", &op, key, &value);
        if (parsed < 1) {
            fprintf(fout, "Ошибка: не удалось разобрать команду\n\n");
            continue;
        }

        if (op == 1) {
            if (parsed < 3) {
                fprintf(fout, "Ошибка: операция 1 требует ключ и значение\n\n");
                continue;
            }
            process_command(tree, 1, key, value, fout);
        } else if (op == 2) {
            if (parsed < 2) {
                fprintf(fout, "Ошибка: операция 2 требует ключ\n\n");
                continue;
            }
            process_command(tree, 2, key, 0.0, fout);
        } else if (op == 3) {
            process_command(tree, 3, NULL, 0.0, fout);
        } else if (op == 4) {
            if (parsed < 2) {
                fprintf(fout, "Ошибка: операция 4 требует ключ\n\n");
                continue;
            }
            process_command(tree, 4, key, 0.0, fout);
        } else {
            process_command(tree, op, key, 0.0, fout);
        }
        fprintf(fout, "\n");
    }

    btree_free(tree);
    fclose(fin);
    if (fout != stdout) fclose(fout);
}

static void run_interactive(void) {
    BTree *tree = btree_create();
    char key[KEY_LEN];
    double value;
    int op;

    printf("Лабораторная работа 4. B-дерево (t=2, 2-3-4 дерево).\n");
    printf("  1 <ключ> <значение> - добавить\n");
    printf("  2 <ключ>            - удалить\n");
    printf("  3                   - вывести дерево\n");
    printf("  4 <ключ>            - найти\n");
    printf("  0                   - выход\n\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (op == 0) break;
        if (op == 1) {
            if (scanf("%6s %lf", key, &value) != 2) {
                printf("Ошибка ввода\n");
                while (getchar() != '\n');
                continue;
            }
            process_command(tree, 1, key, value, stdout);
        } else if (op == 2 || op == 4) {
            if (scanf("%6s", key) != 1) {
                printf("Ошибка ввода\n");
                while (getchar() != '\n');
                continue;
            }
            process_command(tree, op, key, 0.0, stdout);
        } else if (op == 3) {
            process_command(tree, 3, NULL, 0.0, stdout);
        } else {
            printf("Неизвестная операция\n");
        }
    }

    btree_free(tree);
    printf("Выход.\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        run_interactive();
    } else if (argc == 2) {
        run_file_mode(argv[1], NULL);
    } else if (argc == 3) {
        run_file_mode(argv[1], argv[2]);
    } else {
        fprintf(stderr, "Использование: %s [входной_файл [выходной_файл]]\n", argv[0]);
        return 1;
    }
    return 0;
}
