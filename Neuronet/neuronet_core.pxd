from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "sparse_graph.h":
    cdef cppclass SparseGraph:
        SparseGraph() except +
        void load_from_edge_list(const string& path, bint directed)
        int num_nodes() const
        long long num_edges() const
        int node_with_max_degree() const
        vector[int] neighbors(int node) const
        void bfs_subgraph(int source,
                          int max_depth,
                          vector[int]& out_nodes,
                          vector[int]& out_edges_src,
                          vector[int]& out_edges_dst) const
