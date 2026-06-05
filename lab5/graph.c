#include "graph.h"

#include <stdlib.h>
#include <string.h>

Graph *graph_create(int V) {
    if (V <= 0 || V > MAX_VERTICES) return NULL;

    Graph *g = (Graph *)calloc(1, sizeof(Graph));
    if (!g) return NULL;

    g->V = V;
    g->E = 0;
    g->capacity = 16;
    g->src = (int *)malloc(g->capacity * sizeof(int));
    g->dst = (int *)malloc(g->capacity * sizeof(int));
    g->weight = (double *)malloc(g->capacity * sizeof(double));
    g->inc = (double *)calloc((size_t)V * g->capacity, sizeof(double));

    if (!g->src || !g->dst || !g->weight || !g->inc) {
        graph_free(g);
        return NULL;
    }
    return g;
}

void graph_free(Graph *g) {
    if (!g) return;
    free(g->src);
    free(g->dst);
    free(g->weight);
    free(g->inc);
    free(g);
}

static int grow_capacity(Graph *g) {
    int new_cap = g->capacity * 2;

    int *ns  = (int *)realloc(g->src, new_cap * sizeof(int));
    int *nd  = (int *)realloc(g->dst, new_cap * sizeof(int));
    double *nw = (double *)realloc(g->weight, new_cap * sizeof(double));
    if (!ns || !nd || !nw) {
        if (ns) g->src = ns;
        if (nd) g->dst = nd;
        if (nw) g->weight = nw;
        return 0;
    }
    g->src = ns;
    g->dst = nd;
    g->weight = nw;

    double *ni = (double *)calloc((size_t)g->V * new_cap, sizeof(double));
    if (!ni) return 0;
    for (int r = 0; r < g->V; r++)
        memcpy(ni + (size_t)r * new_cap, g->inc + (size_t)r * g->capacity,
               g->E * sizeof(double));
    free(g->inc);
    g->inc = ni;
    g->capacity = new_cap;
    return 1;
}

int graph_add_edge(Graph *g, int from, int to, double w) {
    if (!g) return 0;
    if (from < 0 || from >= g->V || to < 0 || to >= g->V) return 0;

    if (g->E >= g->capacity && !grow_capacity(g)) return 0;

    int e = g->E;
    g->src[e] = from;
    g->dst[e] = to;
    g->weight[e] = w;

    g->inc[(size_t)from * g->capacity + e] =  w;
    g->inc[(size_t)to   * g->capacity + e] = -w;

    g->E++;
    return 1;
}

int graph_remove_edge(Graph *g, int idx) {
    if (!g || idx < 0 || idx >= g->E) return 0;

    int old_src = g->src[idx];
    int old_dst = g->dst[idx];
    g->inc[(size_t)old_src * g->capacity + idx] = 0.0;
    g->inc[(size_t)old_dst * g->capacity + idx] = 0.0;

    int last = g->E - 1;
    if (idx != last) {
        g->src[idx] = g->src[last];
        g->dst[idx] = g->dst[last];
        g->weight[idx] = g->weight[last];

        for (int v = 0; v < g->V; v++) {
            g->inc[(size_t)v * g->capacity + idx] =
                g->inc[(size_t)v * g->capacity + last];
            g->inc[(size_t)v * g->capacity + last] = 0.0;
        }
    }
    g->E--;
    return 1;
}

void graph_print_incidence(const Graph *g, FILE *out) {
    if (!g || g->E == 0) {
        fprintf(out, "Граф пуст.\n");
        return;
    }

    fprintf(out, "Матрица инцидентности (%d вершин, %d рёбер):\n", g->V, g->E);
    fprintf(out, "%4s", "");
    for (int e = 0; e < g->E; e++)
        fprintf(out, " e%-3d", e);
    fprintf(out, "\n");

    for (int v = 0; v < g->V; v++) {
        fprintf(out, "v%-3d", v);
        for (int e = 0; e < g->E; e++) {
            double val = g->inc[(size_t)v * g->capacity + e];
            if (val == 0.0)
                fprintf(out, " %4s", "0");
            else
                fprintf(out, " %4.1f", val);
        }
        fprintf(out, "\n");
    }
}

double *graph_floyd(const Graph *g, int **next_out) {
    if (!g) return NULL;

    int V = g->V;
    size_t sz = (size_t)V * V;

    double *dist = (double *)malloc(sz * sizeof(double));
    int *next    = (int *)malloc(sz * sizeof(int));
    if (!dist || !next) {
        free(dist);
        free(next);
        return NULL;
    }

    for (size_t i = 0; i < sz; i++) {
        dist[i] = INF;
        next[i] = -1;
    }
    for (int i = 0; i < V; i++)
        dist[(size_t)i * V + i] = 0.0;

    for (int e = 0; e < g->E; e++) {
        int u = g->src[e];
        int v = g->dst[e];
        double w = g->weight[e];
        if (w < dist[(size_t)u * V + v]) {
            dist[(size_t)u * V + v] = w;
            next[(size_t)u * V + v] = v;
        }
    }

    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                double through_k = dist[(size_t)i * V + k] + dist[(size_t)k * V + j];
                if (through_k < dist[(size_t)i * V + j]) {
                    dist[(size_t)i * V + j] = through_k;
                    next[(size_t)i * V + j] = next[(size_t)i * V + k];
                }
            }
        }
    }

    if (next_out)
        *next_out = next;
    else
        free(next);

    return dist;
}

void graph_print_dist(const double *dist, int V, FILE *out) {
    fprintf(out, "Матрица кратчайших расстояний:\n");
    fprintf(out, "%4s", "");
    for (int j = 0; j < V; j++)
        fprintf(out, " v%-5d", j);
    fprintf(out, "\n");

    for (int i = 0; i < V; i++) {
        fprintf(out, "v%-3d", i);
        for (int j = 0; j < V; j++) {
            double d = dist[(size_t)i * V + j];
            if (d >= INF / 2)
                fprintf(out, " %-6s", "INF");
            else
                fprintf(out, " %-6.1f", d);
        }
        fprintf(out, "\n");
    }
}

int graph_get_path(const int *next, int V, int from, int to,
                   int *path, int *path_len)
{
    if (!next || from < 0 || from >= V || to < 0 || to >= V) return 0;
    if (next[(size_t)from * V + to] == -1) return 0;

    int len = 0;
    int cur = from;
    while (cur != to) {
        if (len >= V) return 0;
        path[len++] = cur;
        cur = next[(size_t)cur * V + to];
        if (cur == -1) return 0;
    }
    path[len++] = to;
    *path_len = len;
    return 1;
}
