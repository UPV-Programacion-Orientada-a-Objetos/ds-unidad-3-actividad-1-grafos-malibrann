#ifndef SPARSE_GRAPH_H
#define SPARSE_GRAPH_H

#include "igraph.h"
#include <vector>
#include <string>

class SparseGraph : public IGraph {
public:
    SparseGraph();
    virtual ~SparseGraph() {}

    void load_from_edge_list(const std::string& path,
                             bool directed = true) override;

    int num_nodes() const override { return n_nodes; }
    long long num_edges() const override { return n_edges; }

    int node_with_max_degree() const override;
    std::vector<int> neighbors(int node) const override;

    void bfs_subgraph(int source,
                      int max_depth,
                      std::vector<int>& out_nodes,
                      std::vector<int>& out_edges_src,
                      std::vector<int>& out_edges_dst) const override;

private:
    int n_nodes;
    long long n_edges;
    bool directed_graph;

    std::vector<int> row_ptr;
    std::vector<int> col_idx;
    std::vector<int> out_degree;

    void build_csr_from_edges(const std::vector<std::pair<int,int>>& edges);
};

#endif // SPARSE_GRAPH_H
