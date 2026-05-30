extern "C" {
#include "graph.h"
}

#include <gtest/gtest.h>
#include <cstdlib>

TEST(FloydWarshall, SimplePath) {
    Graph *g = graph_create(4);
    ASSERT_NE(g, nullptr);

    graph_add_edge(g, 0, 1, 1.0);
    graph_add_edge(g, 1, 2, 2.0);
    graph_add_edge(g, 2, 3, 3.0);
    graph_add_edge(g, 0, 3, 10.0);

    int *next = NULL;
    double *dist = graph_floyd(g, &next);
    ASSERT_NE(dist, nullptr);

    EXPECT_DOUBLE_EQ(dist[0 * 4 + 1], 1.0);
    EXPECT_DOUBLE_EQ(dist[0 * 4 + 2], 3.0);
    EXPECT_DOUBLE_EQ(dist[0 * 4 + 3], 6.0);
    EXPECT_DOUBLE_EQ(dist[1 * 4 + 2], 2.0);
    EXPECT_DOUBLE_EQ(dist[1 * 4 + 3], 5.0);

    free(dist);
    free(next);
    graph_free(g);
}

TEST(FloydWarshall, NoPath) {
    Graph *g = graph_create(4);
    ASSERT_NE(g, nullptr);

    graph_add_edge(g, 0, 1, 5.0);
    graph_add_edge(g, 2, 3, 7.0);

    double *dist = graph_floyd(g, NULL);
    ASSERT_NE(dist, nullptr);

    EXPECT_GE(dist[0 * 4 + 2], INF / 2);
    EXPECT_GE(dist[0 * 4 + 3], INF / 2);
    EXPECT_GE(dist[1 * 4 + 2], INF / 2);
    EXPECT_GE(dist[2 * 4 + 0], INF / 2);

    EXPECT_DOUBLE_EQ(dist[0 * 4 + 1], 5.0);
    EXPECT_DOUBLE_EQ(dist[2 * 4 + 3], 7.0);

    free(dist);
    graph_free(g);
}

TEST(FloydWarshall, NegativeCycleDetection) {
    Graph *g = graph_create(3);
    ASSERT_NE(g, nullptr);

    graph_add_edge(g, 0, 1, 1.0);
    graph_add_edge(g, 1, 2, -3.0);
    graph_add_edge(g, 2, 0, 1.0);

    double *dist = graph_floyd(g, NULL);
    ASSERT_NE(dist, nullptr);

    int has_negative_cycle = 0;
    for (int i = 0; i < 3; i++) {
        if (dist[i * 3 + i] < 0.0) {
            has_negative_cycle = 1;
            break;
        }
    }
    EXPECT_EQ(has_negative_cycle, 1);

    free(dist);
    graph_free(g);
}

TEST(FloydWarshall, SingleVertex) {
    Graph *g = graph_create(1);
    ASSERT_NE(g, nullptr);

    double *dist = graph_floyd(g, NULL);
    ASSERT_NE(dist, nullptr);

    EXPECT_DOUBLE_EQ(dist[0], 0.0);

    free(dist);
    graph_free(g);
}

TEST(FloydWarshall, PathReconstruction) {
    Graph *g = graph_create(5);
    ASSERT_NE(g, nullptr);

    graph_add_edge(g, 0, 1, 2.0);
    graph_add_edge(g, 1, 2, 3.0);
    graph_add_edge(g, 2, 3, 1.0);
    graph_add_edge(g, 3, 4, 4.0);
    graph_add_edge(g, 0, 4, 100.0);

    int *next = NULL;
    double *dist = graph_floyd(g, &next);
    ASSERT_NE(dist, nullptr);
    ASSERT_NE(next, nullptr);

    EXPECT_DOUBLE_EQ(dist[0 * 5 + 4], 10.0);

    int path[MAX_VERTICES];
    int path_len = 0;
    int ok = graph_get_path(next, 5, 0, 4, path, &path_len);
    ASSERT_EQ(ok, 1);
    ASSERT_EQ(path_len, 5);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[1], 1);
    EXPECT_EQ(path[2], 2);
    EXPECT_EQ(path[3], 3);
    EXPECT_EQ(path[4], 4);

    free(dist);
    free(next);
    graph_free(g);
}
