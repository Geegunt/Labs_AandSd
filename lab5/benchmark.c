#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "graph.h"

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

static void print_platform(void) {
    printf("=== Платформа ===\n");

#ifdef __APPLE__
    char cpu_brand[256] = {0};
    size_t len = sizeof(cpu_brand);
    if (sysctlbyname("machdep.cpu.brand_string", cpu_brand, &len, NULL, 0) == 0)
        printf("CPU: %s\n", cpu_brand);

    int64_t memsize = 0;
    len = sizeof(memsize);
    if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) == 0)
        printf("RAM: %.1f GB\n", (double)memsize / (1024.0 * 1024.0 * 1024.0));
#else
    printf("CPU/RAM: см. /proc/cpuinfo, /proc/meminfo\n");
#endif

    printf("\n");
}

static double bench_floyd(int V, int edge_ratio) {
    Graph *g = graph_create(V);
    if (!g) return -1.0;

    srand(42);
    int target_edges = V * edge_ratio;
    for (int i = 0; i < target_edges; i++) {
        int u = rand() % V;
        int v = rand() % V;
        if (u == v) continue;
        double w = 1.0 + (rand() % 100);
        graph_add_edge(g, u, v, w);
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    double *dist = graph_floyd(g, NULL);

    clock_gettime(CLOCK_MONOTONIC, &t1);

    free(dist);
    graph_free(g);

    double elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0
                   + (t1.tv_nsec - t0.tv_nsec) / 1e6;
    return elapsed;
}

int main(void) {
    print_platform();

    int sizes[] = {10, 50, 100, 200, 500};
    int n = sizeof(sizes) / sizeof(sizes[0]);
    int edge_ratio = 3;

    printf("=== Бенчмарк алгоритма Флойда-Уоршелла ===\n");
    printf("Рёбер на вершину: ~%d (ориентированный граф)\n\n", edge_ratio);

    printf("%-10s %-10s %-15s %-15s\n", "V", "E (прибл.)", "Время (мс)", "O(V^3) отн.");

    double base_time = -1;

    for (int i = 0; i < n; i++) {
        int V = sizes[i];
        int E = V * edge_ratio;
        double ms = bench_floyd(V, edge_ratio);

        if (base_time < 0) base_time = ms;

        double ratio = ms / base_time;
        double expected = ((double)V / sizes[0]) * ((double)V / sizes[0]) * ((double)V / sizes[0]);

        printf("%-10d %-10d %-15.3f %-10.1f (теор. %.1f)\n",
               V, E, ms, ratio, expected);
    }

    printf("\nТеоретическая сложность: O(V^3)\n");
    printf("Колонка \"O(V^3) отн.\" — фактическое отношение времени к базовому (V=%d).\n", sizes[0]);
    printf("Колонка \"теор.\" — ожидаемое отношение при O(V^3).\n");

    return 0;
}
