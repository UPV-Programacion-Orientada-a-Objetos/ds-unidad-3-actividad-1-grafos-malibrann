#include "sparse_graph.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <limits>

SparseGraph::SparseGraph()
    : n_nodes(0), n_edges(0), directed_graph(true) {}

void SparseGraph::load_from_edge_list(const std::string& path,
                                      bool directed) {
    directed_graph = directed;

    std::cout << "[C++ Core] Inicializando GrafoDisperso..." << std::endl;
    std::cout << "[C++ Core] Cargando dataset '" << path << "'..." << std::endl;

    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::vector<std::pair<int,int>> edges;
    edges.reserve(1000000);

    int u, v;
    int max_node = -1;
    long long edge_count = 0;

    while (in >> u >> v) {
        if (u < 0 || v < 0) {
            continue;
        }
        edges.emplace_back(u, v);
        if (!directed) {
            edges.emplace_back(v, u);
        }

        if (u > max_node) max_node = u;
        if (v > max_node) max_node = v;
        edge_count++;
    }
    in.close();

    n_nodes = max_node + 1;
    n_edges = directed ? edge_count : static_cast<long long>(edges.size());

    build_csr_from_edges(edges);

    std::size_t mem_bytes =
        row_ptr.size() * sizeof(int) +
        col_idx.size() * sizeof(int) +
        out_degree.size() * sizeof(int);

    double mem_mb = static_cast<double>(mem_bytes) / (1024.0 * 1024.0);

    std::cout << "[C++ Core] Carga completa. Nodos: " << n_nodes
              << " | Aristas: " << n_edges << std::endl;
    std::cout << "[C++ Core] Estructura CSR construida. Memoria estimada: "
              << mem_mb << " MB." << std::endl;
}

void SparseGraph::build_csr_from_edges(const std::vector<std::pair<int,int>>& edges) {
    row_ptr.assign(n_nodes + 1, 0);
    out_degree.assign(n_nodes, 0);

    for (const auto& e : edges) {
        int u = e.first;
        if (u >= 0 && u < n_nodes) {
            out_degree[u]++;
        }
    }

    row_ptr[0] = 0;
    for (int i = 0; i < n_nodes; ++i) {
        row_ptr[i + 1] = row_ptr[i] + out_degree[i];
    }

    col_idx.assign(n_edges, -1);

    std::vector<int> offset(n_nodes, 0);
    for (int i = 0; i < n_nodes; ++i) {
        offset[i] = row_ptr[i];
    }

    for (const auto& e : edges) {
        int u = e.first;
        int v = e.second;
        int pos = offset[u]++;
        col_idx[pos] = v;
    }
}

int SparseGraph::node_with_max_degree() const {
    int best_node = -1;
    int best_deg = -1;
    for (int i = 0; i < n_nodes; ++i) {
        int deg = out_degree[i];
        if (deg > best_deg) {
            best_deg = deg;
            best_node = i;
        }
    }
    return best_node;
}

std::vector<int> SparseGraph::neighbors(int node) const {
    if (node < 0 || node >= n_nodes) return {};
    int start = row_ptr[node];
    int end = row_ptr[node + 1];
    std::vector<int> neigh;
    neigh.reserve(end - start);
    for (int i = start; i < end; ++i) {
        neigh.push_back(col_idx[i]);
    }
    return neigh;
}

void SparseGraph::bfs_subgraph(int source,
                               int max_depth,
                               std::vector<int>& out_nodes,
                               std::vector<int>& out_edges_src,
                               std::vector<int>& out_edges_dst) const {
    if (source < 0 || source >= n_nodes) {
        throw std::runtime_error("Nodo origen fuera de rango");
    }
    if (max_depth < 0) max_depth = 0;

    std::cout << "[C++ Core] Ejecutando BFS nativo..." << std::endl;

    std::vector<int> depth(n_nodes, -1);
    std::queue<int> q;

    depth[source] = 0;
    q.push(source);

    out_nodes.clear();
    out_edges_src.clear();
    out_edges_dst.clear();

    out_nodes.push_back(source);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        int d = depth[u];
        if (d >= max_depth) continue;

        int start = row_ptr[u];
        int end = row_ptr[u + 1];

        for (int i = start; i < end; ++i) {
            int v = col_idx[i];
            if (depth[v] == -1) {
                depth[v] = d + 1;
                q.push(v);
                out_nodes.push_back(v);
                out_edges_src.push_back(u);
                out_edges_dst.push_back(v);
            } else if (depth[v] == d + 1) {
                out_edges_src.push_back(u);
                out_edges_dst.push_back(v);
            }
        }
    }

    std::cout << "[C++ Core] Nodos encontrados: " << out_nodes.size()
              << ". Aristas en subgrafo: " << out_edges_src.size()
              << std::endl;
}
