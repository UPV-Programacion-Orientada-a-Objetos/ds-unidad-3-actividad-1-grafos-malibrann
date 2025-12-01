# distutils: language = c++

from libcpp.vector cimport vector
from libcpp.string cimport string
cimport cython

from neuronet_core cimport SparseGraph

cdef class PySparseGraph:
    cdef SparseGraph* _ptr

    def __cinit__(self):
        self._ptr = new SparseGraph()

    def __dealloc__(self):
        if self._ptr is not None:
            del self._ptr
            self._ptr = NULL

    def load_from_edge_list(self, path: str, directed: bool = True):
        cdef string cpath = path.encode('utf-8')
        self._ptr.load_from_edge_list(cpath, directed)

    @property
    def num_nodes(self) -> int:
        return self._ptr.num_nodes()

    @property
    def num_edges(self) -> int:
        return self._ptr.num_edges()

    def node_with_max_degree(self) -> int:
        return self._ptr.node_with_max_degree()

    def neighbors(self, node: int):
        cdef vector[int] v = self._ptr.neighbors(node)
        cdef int i, n = v.size()
        return [v[i] for i in range(n)]

    def bfs_subgraph(self, source: int, max_depth: int = 1):
        cdef vector[int] v_nodes
        cdef vector[int] v_src
        cdef vector[int] v_dst

        self._ptr.bfs_subgraph(source, max_depth, v_nodes, v_src, v_dst)

        cdef int i
        cdef int n_nodes = v_nodes.size()
        cdef int n_edges = v_src.size()

        nodes = [v_nodes[i] for i in range(n_nodes)]
        edges = [(v_src[i], v_dst[i]) for i in range(n_edges)]

        return nodes, edges
