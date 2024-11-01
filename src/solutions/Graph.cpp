#include "Graph.h"

#include <stdexcept>
#include <limits>
#include <queue>

bool Edge::operator>(const Edge &other) const {
    return this->weight > other.weight;
}

// Graph::Graph(std::size_t node_count): node_count(node_count), edges(node_count) {}

void Graph::add_directed_edge(long source, long weight, long destination) {
    if (source == destination) {
        throw std::logic_error("loops unsupported for now");
    }
    this->edges[source].push_back(Edge{weight, destination});
}

void Graph::add_undirected_edge(long source, long weight, long destination) {
    this->add_directed_edge(source, weight, destination);
    this->add_directed_edge(destination, weight, source);
}

std::vector<long> Graph::dijkstra(long start) {
    if (start >= this->node_count) throw std::logic_error("Cannot start outside of graph");

    std::vector<long> distances(this->node_count, std::numeric_limits<long>::max());
    distances[start] = 0;

    std::priority_queue<std::pair<long, long>, std::vector<std::pair<long, long>>, std::greater<>> priority_queue;
    priority_queue.emplace(0l, start);

    while (!priority_queue.empty()) {
        long current_distance = priority_queue.top().first;
        long source = priority_queue.top().second;
        priority_queue.pop();

        if (current_distance > distances[source]) continue;

        for (auto& [weight, destination]: this->edges[source]) {
            if (distances[source] + weight < distances[destination]) {
                distances[destination] = distances[source] + weight;
                priority_queue.emplace(distances[destination], destination);
            }
        }
    }

    return distances;
}