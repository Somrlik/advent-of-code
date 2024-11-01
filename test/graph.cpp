#include <gtest/gtest.h>

#include "../src/solutions/Graph.h"

TEST(Graph, dijkstraSimpleUndirected) {
    auto graph = Graph(6);
    graph.add_undirected_edge(0, 14, 5);
    graph.add_undirected_edge(0, 9, 2);
    graph.add_undirected_edge(0, 7, 1);

    graph.add_undirected_edge(1, 10, 2);
    graph.add_undirected_edge(1, 15, 3);

    graph.add_undirected_edge(2, 2, 5);
    graph.add_undirected_edge(2, 11, 3);

    graph.add_undirected_edge(3, 6, 4);
    graph.add_undirected_edge(4, 9, 5);

    auto distances = graph.dijkstra(0);

    ASSERT_EQ(distances[0], 0);
    ASSERT_EQ(distances[1], 7);
    ASSERT_EQ(distances[2], 9);
    ASSERT_EQ(distances[3], 20);
    ASSERT_EQ(distances[4], 20);
    ASSERT_EQ(distances[5], 11);
}

TEST(Graph, dijkstraSimpleDirected) {
    auto graph = Graph(6);
    graph.add_directed_edge(0, 14, 5);
    graph.add_directed_edge(0, 9, 2);
    graph.add_directed_edge(0, 7, 1);

    graph.add_directed_edge(1, 10, 2);
    graph.add_directed_edge(1, 15, 3);

    graph.add_directed_edge(2, 2, 5);
    graph.add_directed_edge(2, 11, 3);

    graph.add_directed_edge(3, 6, 4);
    graph.add_directed_edge(4, 9, 5);

    auto distances = graph.dijkstra(0);

    ASSERT_EQ(distances[0], 0);
    ASSERT_EQ(distances[1], 7);
    ASSERT_EQ(distances[2], 9);
    ASSERT_EQ(distances[3], 20);
    ASSERT_EQ(distances[4], 26);
    ASSERT_EQ(distances[5], 11);
}