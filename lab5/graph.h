#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

#define INF 1e18
#define MAX_VERTICES 1024

typedef struct {
    int V;
    int E;
    int capacity;
    int *src;
    int *dst;
    double *weight;
    double *inc;
} Graph;

Graph *graph_create(int V);
void   graph_free(Graph *g);

int  graph_add_edge(Graph *g, int from, int to, double w);
int  graph_remove_edge(Graph *g, int idx);

void graph_print_incidence(const Graph *g, FILE *out);

double *graph_floyd(const Graph *g, int **next_out);
void    graph_print_dist(const double *dist, int V, FILE *out);
int     graph_get_path(const int *next, int V, int from, int to,
                       int *path, int *path_len);

#endif
