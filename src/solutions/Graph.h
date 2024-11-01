#ifndef AOC_GRAPH_H
#define AOC_GRAPH_H

#include <vector>

typedef struct Edge {
    long weight;
    long destination;

    bool operator>(const Edge &other) const;
} Edge;

class Graph {
    std::size_t node_count;
    std::vector<std::vector<Edge>> edges;

public:
    explicit Graph(std::size_t node_count): node_count(node_count), edges(node_count) {}

    void add_directed_edge(long source, long weight, long destination);
    void add_undirected_edge(long source, long weight, long destination);

    std::vector<long> dijkstra(long start);
};

#endif
