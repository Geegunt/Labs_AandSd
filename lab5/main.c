#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

static void print_help(void) {
    printf("Лабораторная работа 5. Алгоритм Флойда-Уоршелла.\n");
    printf("Граф хранится в виде матрицы инцидентности.\n\n");
    printf("  1 <from> <to> <weight>  — добавить ребро\n");
    printf("  2 <index>               — удалить ребро по индексу\n");
    printf("  3                       — вывести матрицу инцидентности\n");
    printf("  4                       — запустить Флойда-Уоршелла\n");
    printf("  5 <from> <to>           — кратчайший путь между вершинами\n");
    printf("  0                       — выход\n\n");
}

static void run_floyd_cmd(Graph *g, FILE *out) {
    int *next = NULL;
    double *dist = graph_floyd(g, &next);
    if (!dist) {
        fprintf(out, "Ошибка вычисления.\n");
        return;
    }
    graph_print_dist(dist, g->V, out);
    free(dist);
    free(next);
}

static void run_path_cmd(Graph *g, int from, int to, FILE *out) {
    int *next = NULL;
    double *dist = graph_floyd(g, &next);
    if (!dist) {
        fprintf(out, "Ошибка вычисления.\n");
        return;
    }

    if (from < 0 || from >= g->V || to < 0 || to >= g->V) {
        fprintf(out, "Ошибка: вершина вне диапазона [0, %d)\n", g->V);
        free(dist);
        free(next);
        return;
    }

    double d = dist[(size_t)from * g->V + to];
    if (d >= INF / 2) {
        fprintf(out, "Пути из %d в %d не существует.\n", from, to);
    } else {
        int path[MAX_VERTICES];
        int path_len = 0;
        if (graph_get_path(next, g->V, from, to, path, &path_len)) {
            fprintf(out, "Кратчайший путь из %d в %d (длина %.1f): ", from, to, d);
            for (int i = 0; i < path_len; i++) {
                if (i > 0) fprintf(out, " -> ");
                fprintf(out, "%d", path[i]);
            }
            fprintf(out, "\n");
        }
    }
    free(dist);
    free(next);
}

static void process_command(Graph *g, int op, const char *args, FILE *out) {
    int from, to, idx;
    double w;

    switch (op) {
        case 1:
            if (sscanf(args, "%d %d %lf", &from, &to, &w) != 3) {
                fprintf(out, "Ошибка: операция 1 требует <from> <to> <weight>\n");
                break;
            }
            if (from < 0 || from >= g->V || to < 0 || to >= g->V) {
                fprintf(out, "Ошибка: вершина вне диапазона [0, %d)\n", g->V);
                break;
            }
            if (graph_add_edge(g, from, to, w))
                fprintf(out, "Добавлено ребро: %d -> %d (вес %.1f)\n", from, to, w);
            else
                fprintf(out, "Ошибка добавления ребра.\n");
            break;
        case 2:
            if (sscanf(args, "%d", &idx) != 1) {
                fprintf(out, "Ошибка: операция 2 требует <index>\n");
                break;
            }
            if (graph_remove_edge(g, idx))
                fprintf(out, "Ребро %d удалено.\n", idx);
            else
                fprintf(out, "Ошибка: ребро %d не найдено.\n", idx);
            break;
        case 3:
            graph_print_incidence(g, out);
            break;
        case 4:
            run_floyd_cmd(g, out);
            break;
        case 5:
            if (sscanf(args, "%d %d", &from, &to) != 2) {
                fprintf(out, "Ошибка: операция 5 требует <from> <to>\n");
                break;
            }
            run_path_cmd(g, from, to, out);
            break;
        default:
            fprintf(out, "Неизвестная операция: %d\n", op);
    }
}

static void run_file_mode(int V, const char *in_path, const char *out_path) {
    FILE *fin = fopen(in_path, "r");
    if (!fin) {
        fprintf(stderr, "Ошибка: не удалось открыть '%s'\n", in_path);
        return;
    }
    FILE *fout = out_path ? fopen(out_path, "w") : stdout;
    if (!fout) {
        fprintf(stderr, "Ошибка: не удалось открыть '%s' для записи\n", out_path);
        fclose(fin);
        return;
    }

    Graph *g = graph_create(V);
    if (!g) {
        fprintf(fout, "Ошибка создания графа.\n");
        fclose(fin);
        if (fout != stdout) fclose(fout);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fin)) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        fprintf(fout, "> %s\n", line);

        int op;
        int parsed = sscanf(line, "%d", &op);
        if (parsed < 1) {
            fprintf(fout, "Ошибка: не удалось разобрать команду\n\n");
            continue;
        }

        char *args = line;
        while (*args && *args != ' ') args++;
        while (*args == ' ') args++;

        process_command(g, op, args, fout);
        fprintf(fout, "\n");
    }

    graph_free(g);
    fclose(fin);
    if (fout != stdout) fclose(fout);
}

static void run_interactive(int V) {
    Graph *g = graph_create(V);
    if (!g) {
        printf("Ошибка создания графа.\n");
        return;
    }

    print_help();

    char line[256];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;

        int op;
        if (sscanf(line, "%d", &op) != 1) continue;
        if (op == 0) break;

        char *args = line;
        while (*args && *args != ' ') args++;
        while (*args == ' ') args++;

        process_command(g, op, args, stdout);
    }

    graph_free(g);
    printf("Выход.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <кол-во вершин> [входной_файл [выходной_файл]]\n",
                argv[0]);
        return 1;
    }

    int V = atoi(argv[1]);
    if (V <= 0) {
        fprintf(stderr, "Ошибка: количество вершин должно быть > 0\n");
        return 1;
    }

    if (argc == 2)
        run_interactive(V);
    else if (argc == 3)
        run_file_mode(V, argv[2], NULL);
    else if (argc == 4)
        run_file_mode(V, argv[2], argv[3]);
    else {
        fprintf(stderr, "Слишком много аргументов.\n");
        return 1;
    }

    return 0;
}
