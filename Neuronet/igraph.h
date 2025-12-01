#ifndef IGRAPH_H
#define IGRAPH_H

#include <string>
#include <vector>

class IGraph {
public:
    virtual ~IGraph() {}

    virtual void load_from_edge_list(const std::string& path,
                                     bool directed = true) = 0;

    virtual int num_nodes() const = 0;
    virtual long long num_edges() const = 0;

    virtual int node_with_max_degree() const = 0;

    virtual std::vector<int> neighbors(int node) const = 0;

    virtual void bfs_subgraph(int source,
                              int max_depth,
                              std::vector<int>& out_nodes,
                              std::vector<int>& out_edges_src,
                              std::vector<int>& out_edges_dst) const = 0;
};

#endif // IGRAPH_H
