import tkinter as tk
from tkinter import filedialog, messagebox

import matplotlib.pyplot as plt
import networkx as nx
import os
import time

from neuronet_core import PySparseGraph


class NeuroNetApp:
    def __init__(self, root):
        self.root = root
        self.root.title("NeuroNet - Global Connectivity Watch")

        self.graph = None
        self.current_path = None

        top_frame = tk.Frame(root)
        top_frame.pack(fill=tk.X, padx=10, pady=5)

        self.btn_load = tk.Button(
            top_frame, text="Cargar dataset SNAP", command=self.load_dataset
        )
        self.btn_load.pack(side=tk.LEFT)

        self.lbl_info = tk.Label(top_frame, text="Nodos: - | Aristas: - | Archivo: -")
        self.lbl_info.pack(side=tk.LEFT, padx=10)

        mid_frame = tk.Frame(root)
        mid_frame.pack(fill=tk.X, padx=10, pady=5)

        self.btn_critical = tk.Button(
            mid_frame, text="Nodo más crítico (mayor grado)", command=self.show_critical_node
        )
        self.btn_critical.grid(row=0, column=0, padx=5, pady=5)

        self.lbl_critical = tk.Label(mid_frame, text="Nodo crítico: -")
        self.lbl_critical.grid(row=0, column=1, padx=5, pady=5, sticky="w")

        tk.Label(mid_frame, text="Nodo inicio:").grid(row=1, column=0, sticky="e")
        self.entry_source = tk.Entry(mid_frame, width=10)
        self.entry_source.grid(row=1, column=1, sticky="w")

        tk.Label(mid_frame, text="Profundidad máxima:").grid(row=1, column=2, sticky="e")
        self.entry_depth = tk.Entry(mid_frame, width=10)
        self.entry_depth.grid(row=1, column=3, sticky="w")

        self.btn_bfs = tk.Button(
            mid_frame, text="Ejecutar BFS y visualizar", command=self.run_bfs_and_plot
        )
        self.btn_bfs.grid(row=1, column=4, padx=5, pady=5)

        bottom_frame = tk.Frame(root)
        bottom_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        tk.Label(bottom_frame, text="Log:").pack(anchor="w")
        self.text_log = tk.Text(bottom_frame, height=10, state=tk.DISABLED)
        self.text_log.pack(fill=tk.BOTH, expand=True)

    def log(self, msg: str):
        self.text_log.config(state=tk.NORMAL)
        self.text_log.insert(tk.END, msg + "\n")
        self.text_log.config(state=tk.DISABLED)
        self.text_log.see(tk.END)

    def load_dataset(self):
        path = filedialog.askopenfilename(
            title="Seleccionar archivo SNAP (edge list)",
            filetypes=[("Text files", "*.txt *.edges *.dat *.csv"), ("All files", "*.*")]
        )
        if not path:
            return

        try:
            self.log(f"[Python GUI] Seleccionado archivo: {path}")
            t0 = time.time()
            self.graph = PySparseGraph()
            self.graph.load_from_edge_list(path, directed=True)
            t1 = time.time()
            dt = (t1 - t0) * 1000.0

            self.current_path = os.path.basename(path)
            n = self.graph.num_nodes
            m = self.graph.num_edges

            self.lbl_info.config(
                text=f"Nodos: {n} | Aristas: {m} | Archivo: {self.current_path}"
            )
            self.log(f"[Python GUI] Carga completada en {dt:.2f} ms.")
            self.log(f"[Cython] Núcleo listo para análisis.")

        except Exception as e:
            messagebox.showerror("Error", str(e))
            self.log(f"[Error] {e}")

    def show_critical_node(self):
        if self.graph is None:
            messagebox.showwarning("Aviso", "Carga primero un dataset.")
            return
        nid = self.graph.node_with_max_degree()
        self.lbl_critical.config(text=f"Nodo crítico: {nid}")
        self.log(f"[Cython] Solicitud: Nodo más crítico.")
        self.log(f"[Python GUI] Nodo con mayor grado: {nid}")

    def run_bfs_and_plot(self):
        if self.graph is None:
            messagebox.showwarning("Aviso", "Carga primero un dataset.")
            return

        try:
            source = int(self.entry_source.get())
            depth = int(self.entry_depth.get())
        except ValueError:
            messagebox.showerror("Error", "Nodo inicio y profundidad deben ser enteros.")
            return

        self.log(f"[Cython] Solicitud recibida: BFS desde nodo {source}, profundidad {depth}.")
        t0 = time.time()
        nodes, edges = self.graph.bfs_subgraph(source, depth)
        t1 = time.time()
        dt = (t1 - t0) * 1000.0

        self.log(f"[Python GUI] Subgrafo BFS: {len(nodes)} nodos, {len(edges)} aristas. Tiempo: {dt:.4f} ms.")

        G = nx.DiGraph()
        G.add_nodes_from(nodes)
        G.add_edges_from(edges)

        plt.figure(figsize=(6, 6))
        pos = nx.spring_layout(G, k=0.5, iterations=50)
        nx.draw(G, pos, with_labels=True, node_size=300, font_size=8, arrows=True)
        plt.title(f"Subgrafo BFS desde {source}, profundidad {depth}")
        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    root = tk.Tk()
    app = NeuroNetApp(root)
    root.mainloop()
