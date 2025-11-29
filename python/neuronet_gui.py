"""
neuronet_gui.py - Interfaz gráfica para NeuroNet usando Tkinter + PyVis

Este módulo proporciona una GUI completa para:
- Cargar grafos masivos desde archivos SNAP
- Visualizar subgrafos usando PyVis (interactivo en navegador)
- Ejecutar algoritmos BFS/DFS
- Analizar métricas del grafo
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import threading
import webbrowser
import os
import tempfile
from typing import Optional, List, Tuple

# PyVis para visualización interactiva
from pyvis.network import Network

# Importar el wrapper de C++ (después de compilar)
try:
    from grafo_wrapper import PyGrafoDisperso

    GRAFO_DISPONIBLE = True
except ImportError:
    print(
        "ADVERTENCIA: No se pudo importar grafo_wrapper. Ejecuta 'python setup.py build_ext --inplace' primero."
    )
    GRAFO_DISPONIBLE = False


class NeuroNetGUI:
    """
    Interfaz gráfica principal de NeuroNet.

    Permite cargar grafos masivos y visualizarlos de forma interactiva
    usando PyVis, que genera HTML navegable.
    """

    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("NeuroNet - Análisis de Redes Masivas")
        self.root.geometry("900x700")
        self.root.minsize(800, 600)

        # Grafo C++
        self.grafo: Optional[PyGrafoDisperso] = None
        self.archivo_actual = ""

        # Archivo temporal para visualización
        self.temp_dir = tempfile.mkdtemp()
        self.html_file = os.path.join(self.temp_dir, "grafo.html")

        # Crear interfaz
        self._crear_interfaz()

        # Inicializar grafo si está disponible
        if GRAFO_DISPONIBLE:
            self.grafo = PyGrafoDisperso()
            self._log("Sistema inicializado correctamente.")
        else:
            self._log("ERROR: Módulo C++ no disponible. Compila primero con:")
            self._log("  python setup.py build_ext --inplace")

    def _crear_interfaz(self):
        """Crea todos los componentes de la interfaz."""

        # ==================== BARRA SUPERIOR ====================
        frame_superior = ttk.Frame(self.root, padding="10")
        frame_superior.pack(fill=tk.X)

        # Botón cargar archivo
        ttk.Button(
            frame_superior, text="📂 Cargar Grafo", command=self._cargar_archivo
        ).pack(side=tk.LEFT, padx=5)

        # Label archivo actual
        self.label_archivo = ttk.Label(
            frame_superior, text="Ningún archivo cargado", font=("Arial", 9)
        )
        self.label_archivo.pack(side=tk.LEFT, padx=10)

        # ==================== PANEL PRINCIPAL ====================
        panel_principal = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        panel_principal.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Panel izquierdo (controles)
        frame_controles = ttk.Frame(panel_principal, width=300)
        panel_principal.add(frame_controles, weight=1)

        # Panel derecho (estadísticas y log)
        frame_derecho = ttk.Frame(panel_principal)
        panel_principal.add(frame_derecho, weight=2)

        # ==================== ESTADÍSTICAS ====================
        frame_stats = ttk.LabelFrame(frame_controles, text="Estadísticas", padding="10")
        frame_stats.pack(fill=tk.X, pady=5)

        self.stats_text = tk.Text(frame_stats, height=8, width=35, state=tk.DISABLED)
        self.stats_text.pack(fill=tk.X)

        # ==================== VISUALIZACIÓN ====================
        frame_vis = ttk.LabelFrame(frame_controles, text="Visualización", padding="10")
        frame_vis.pack(fill=tk.X, pady=5)

        # Tipo de visualización
        ttk.Label(frame_vis, text="Modo:").pack(anchor=tk.W)
        self.modo_vis = tk.StringVar(value="bfs")
        ttk.Radiobutton(
            frame_vis, text="BFS desde nodo", variable=self.modo_vis, value="bfs"
        ).pack(anchor=tk.W)
        ttk.Radiobutton(
            frame_vis, text="Top K nodos", variable=self.modo_vis, value="topk"
        ).pack(anchor=tk.W)
        ttk.Radiobutton(
            frame_vis, text="Rango de nodos", variable=self.modo_vis, value="rango"
        ).pack(anchor=tk.W)
        ttk.Radiobutton(
            frame_vis,
            text="Muestra aleatoria",
            variable=self.modo_vis,
            value="aleatorio",
        ).pack(anchor=tk.W)

        # Parámetros
        frame_params = ttk.Frame(frame_vis)
        frame_params.pack(fill=tk.X, pady=5)

        ttk.Label(frame_params, text="Nodo inicio:").grid(row=0, column=0, sticky=tk.W)
        self.entry_nodo = ttk.Entry(frame_params, width=10)
        self.entry_nodo.insert(0, "0")
        self.entry_nodo.grid(row=0, column=1, padx=5)

        ttk.Label(frame_params, text="Profundidad/K:").grid(
            row=1, column=0, sticky=tk.W
        )
        self.entry_profundidad = ttk.Entry(frame_params, width=10)
        self.entry_profundidad.insert(0, "2")
        self.entry_profundidad.grid(row=1, column=1, padx=5)

        ttk.Label(frame_params, text="Máx. nodos:").grid(row=2, column=0, sticky=tk.W)
        self.entry_max_nodos = ttk.Entry(frame_params, width=10)
        self.entry_max_nodos.insert(0, "500")
        self.entry_max_nodos.grid(row=2, column=1, padx=5)

        # Slider para cantidad de nodos
        ttk.Label(frame_vis, text="Cantidad de nodos a visualizar:").pack(
            anchor=tk.W, pady=(10, 0)
        )
        self.slider_nodos = ttk.Scale(
            frame_vis, from_=10, to=5000, orient=tk.HORIZONTAL
        )
        self.slider_nodos.set(200)
        self.slider_nodos.pack(fill=tk.X)
        self.label_slider = ttk.Label(frame_vis, text="200 nodos")
        self.label_slider.pack()
        self.slider_nodos.configure(command=self._actualizar_slider)

        # Botón visualizar
        ttk.Button(
            frame_vis, text="Visualizar Subgrafo", command=self._visualizar
        ).pack(fill=tk.X, pady=10)

        # Advertencia
        self.label_advertencia = ttk.Label(
            frame_vis, text="", foreground="orange", wraplength=250
        )
        self.label_advertencia.pack()

        # ==================== ALGORITMOS ====================
        frame_algo = ttk.LabelFrame(frame_controles, text="Algoritmos", padding="10")
        frame_algo.pack(fill=tk.X, pady=5)

        ttk.Button(
            frame_algo,
            text="BFS Completo",
            command=lambda: self._ejecutar_algoritmo("bfs"),
        ).pack(fill=tk.X, pady=2)

        ttk.Button(
            frame_algo,
            text="DFS Completo",
            command=lambda: self._ejecutar_algoritmo("dfs"),
        ).pack(fill=tk.X, pady=2)

        ttk.Button(
            frame_algo, text="Camino más corto", command=self._camino_mas_corto
        ).pack(fill=tk.X, pady=2)

        # ==================== CONSOLA/LOG ====================
        frame_log = ttk.LabelFrame(frame_derecho, text="Consola", padding="10")
        frame_log.pack(fill=tk.BOTH, expand=True, pady=5)

        # Scrollbar
        scrollbar = ttk.Scrollbar(frame_log)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.log_text = tk.Text(
            frame_log,
            height=30,
            width=60,
            yscrollcommand=scrollbar.set,
            bg="#1e1e1e",
            fg="#00ff00",
            font=("Consolas", 9),
        )
        self.log_text.pack(fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.log_text.yview)

        # Botón limpiar log
        ttk.Button(
            frame_derecho,
            text="Limpiar Consola",
            command=lambda: self.log_text.delete(1.0, tk.END),
        ).pack(pady=5)

        # ==================== BARRA DE ESTADO ====================
        self.status_bar = ttk.Label(
            self.root, text="Listo", relief=tk.SUNKEN, anchor=tk.W
        )
        self.status_bar.pack(fill=tk.X, side=tk.BOTTOM, padx=5, pady=2)

        # Barra de progreso
        self.progress = ttk.Progressbar(self.root, mode="indeterminate")

    def _log(self, mensaje: str):
        """Añade un mensaje al log."""
        self.log_text.insert(tk.END, mensaje + "\n")
        self.log_text.see(tk.END)
        self.root.update_idletasks()

    def _actualizar_stats(self):
        """Actualiza el panel de estadísticas."""
        self.stats_text.config(state=tk.NORMAL)
        self.stats_text.delete(1.0, tk.END)

        if self.grafo and self.grafo.num_nodos > 0:
            nodo_max, grado_max = self.grafo.get_nodo_mayor_grado()
            stats = f"""Nodos: {self.grafo.num_nodos:,}
Aristas: {self.grafo.num_aristas:,}
Memoria: {self.grafo.memoria_usada_mb:.2f} MB
Tiempo carga: {self.grafo.tiempo_carga:.2f}s

Nodo más conectado: {nodo_max}
  → Grado: {grado_max:,}
"""
            self.stats_text.insert(tk.END, stats)
        else:
            self.stats_text.insert(tk.END, "No hay grafo cargado")

        self.stats_text.config(state=tk.DISABLED)

    def _actualizar_slider(self, value):
        """Actualiza el label del slider."""
        val = int(float(value))
        self.label_slider.config(text=f"{val} nodos")

        # Advertencia si son muchos nodos
        if val > 2000:
            self.label_advertencia.config(
                text="Más de 2000 nodos puede ser lento de renderizar"
            )
        elif val > 1000:
            self.label_advertencia.config(text="Puede tardar unos segundos")
        else:
            self.label_advertencia.config(text="")

    def _cargar_archivo(self):
        """Abre diálogo para cargar archivo de grafo."""
        if not GRAFO_DISPONIBLE:
            messagebox.showerror("Error", "Módulo C++ no disponible")
            return

        archivo = filedialog.askopenfilename(
            title="Seleccionar archivo de grafo",
            filetypes=[("Archivos de texto", "*.txt"), ("Todos los archivos", "*.*")],
        )

        if archivo:
            self._cargar_grafo_async(archivo)

    def _cargar_grafo_async(self, archivo: str):
        """Carga el grafo en un hilo separado."""
        self.status_bar.config(text="Cargando grafo...")
        self.progress.pack(fill=tk.X, side=tk.BOTTOM, padx=5)
        self.progress.start()

        def cargar():
            try:
                self._log(f"\n{'='*50}")
                self._log(f"Cargando: {archivo}")
                self._log(f"{'='*50}")

                exito = self.grafo.cargar_datos(archivo)

                self.root.after(0, lambda: self._cargar_completado(exito, archivo))
            except Exception as e:
                self.root.after(0, lambda: self._cargar_error(str(e)))

        threading.Thread(target=cargar, daemon=True).start()

    def _cargar_completado(self, exito: bool, archivo: str):
        """Callback cuando termina la carga."""
        self.progress.stop()
        self.progress.pack_forget()

        if exito:
            self.archivo_actual = archivo
            self.label_archivo.config(text=os.path.basename(archivo))
            self._actualizar_stats()
            self.status_bar.config(
                text=f"Grafo cargado: {self.grafo.num_nodos:,} nodos, {self.grafo.num_aristas:,} aristas"
            )
            self._log("\nGrafo cargado exitosamente!")
        else:
            self.status_bar.config(text="Error al cargar el grafo")
            messagebox.showerror("Error", "No se pudo cargar el archivo")

    def _cargar_error(self, error: str):
        """Callback cuando hay error en la carga."""
        self.progress.stop()
        self.progress.pack_forget()
        self.status_bar.config(text="Error")
        self._log(f"Error: {error}")
        messagebox.showerror("Error", error)

    def _visualizar(self):
        """Visualiza el subgrafo según el modo seleccionado."""
        if not self.grafo or self.grafo.num_nodos == 0:
            messagebox.showwarning("Advertencia", "Primero carga un grafo")
            return

        try:
            modo = self.modo_vis.get()
            max_nodos = int(self.entry_max_nodos.get())

            self._log(f"\n{'='*50}")
            self._log(f"Generando visualización ({modo})...")

            nodos = []
            aristas = []

            if modo == "bfs":
                nodo_inicio = int(self.entry_nodo.get())
                profundidad = int(self.entry_profundidad.get())

                if nodo_inicio >= self.grafo.num_nodos:
                    messagebox.showerror("Error", f"Nodo {nodo_inicio} no existe")
                    return

                nodos_con_nivel = self.grafo.bfs(nodo_inicio, profundidad)
                nodos = [n[0] for n in nodos_con_nivel[:max_nodos]]
                aristas = self.grafo.get_aristas_subgrafo(nodos)

            elif modo == "topk":
                k = int(self.entry_profundidad.get())
                top_k = self.grafo.get_top_k_nodos(min(k, max_nodos))
                nodos = [n[0] for n in top_k]
                aristas = self.grafo.get_aristas_subgrafo(nodos)

            elif modo == "rango":
                inicio = int(self.entry_nodo.get())
                cantidad = int(self.slider_nodos.get())
                nodos = self.grafo.get_nodos_en_rango(inicio, inicio + cantidad)[
                    :max_nodos
                ]
                aristas = self.grafo.get_aristas_subgrafo(nodos)

            elif modo == "aleatorio":
                cantidad = int(self.slider_nodos.get())
                nodos = self.grafo.muestreo_aleatorio(min(cantidad, max_nodos))
                aristas = self.grafo.get_aristas_subgrafo(nodos)

            self._log(f"Nodos a visualizar: {len(nodos)}")
            self._log(f"Aristas a visualizar: {len(aristas)}")

            # Crear visualización con PyVis
            self._crear_pyvis(nodos, aristas)

        except ValueError as e:
            messagebox.showerror("Error", f"Valor inválido: {e}")
        except Exception as e:
            self._log(f"Error: {e}")
            messagebox.showerror("Error", str(e))

    def _crear_pyvis(self, nodos: List[int], aristas: List[Tuple[int, int]]):
        """Crea y abre la visualización con PyVis."""
        self.status_bar.config(text="Generando visualización...")
        self._log("Creando visualización PyVis...")

        # Configurar PyVis
        net = Network(
            height="800px",
            width="100%",
            bgcolor="#222222",
            font_color="white",
            directed=True,
            notebook=False,
        )

        # Configurar física para mejor layout
        net.set_options(
            """
        {
            "nodes": {
                "borderWidth": 2,
                "borderWidthSelected": 4,
                "font": {
                    "size": 12
                }
            },
            "edges": {
                "color": {
                    "inherit": true
                },
                "smooth": {
                    "type": "continuous"
                },
                "arrows": {
                    "to": {
                        "enabled": true,
                        "scaleFactor": 0.5
                    }
                }
            },
            "physics": {
                "forceAtlas2Based": {
                    "gravitationalConstant": -50,
                    "centralGravity": 0.01,
                    "springLength": 100,
                    "springConstant": 0.08
                },
                "maxVelocity": 50,
                "solver": "forceAtlas2Based",
                "timestep": 0.35,
                "stabilization": {
                    "enabled": true,
                    "iterations": 150
                }
            },
            "interaction": {
                "navigationButtons": true,
                "keyboard": true,
                "hover": true,
                "tooltipDelay": 200
            }
        }
        """
        )

        # Calcular tamaños basados en grado
        grados = {}
        for nodo in nodos:
            grados[nodo] = self.grafo.get_grado_salida(nodo)

        max_grado = max(grados.values()) if grados else 1

        # Añadir nodos
        for nodo in nodos:
            grado = grados[nodo]
            # Tamaño proporcional al grado
            size = 10 + (grado / max_grado) * 30
            # Color basado en grado
            color = self._color_por_grado(grado, max_grado)

            net.add_node(
                nodo,
                label=str(nodo),
                size=size,
                color=color,
                title=f"Nodo: {nodo}\nGrado salida: {grado}\nGrado entrada: {self.grafo.get_grado_entrada(nodo)}",
            )

        # Añadir aristas
        for origen, destino in aristas:
            net.add_edge(origen, destino)

        # Guardar y abrir
        self._log(f"Guardando en: {self.html_file}")
        net.save_graph(self.html_file)

        # Abrir en navegador
        webbrowser.open("file://" + os.path.realpath(self.html_file))

        self.status_bar.config(
            text=f"Visualización generada: {len(nodos)} nodos, {len(aristas)} aristas"
        )
        self._log("Visualización abierta en navegador")
        self._log("   → Usa el mouse para hacer zoom y arrastrar")
        self._log("   → Haz clic en los nodos para ver información")

    def _color_por_grado(self, grado: int, max_grado: int) -> str:
        """Genera un color basado en el grado del nodo."""
        if max_grado == 0:
            return "#97c2fc"

        ratio = grado / max_grado

        if ratio > 0.8:
            return "#ff0000"  # Rojo - muy conectado
        elif ratio > 0.6:
            return "#ff6600"  # Naranja
        elif ratio > 0.4:
            return "#ffcc00"  # Amarillo
        elif ratio > 0.2:
            return "#66ff66"  # Verde claro
        else:
            return "#97c2fc"  # Azul - poco conectado

    def _ejecutar_algoritmo(self, algoritmo: str):
        """Ejecuta BFS o DFS completo."""
        if not self.grafo or self.grafo.num_nodos == 0:
            messagebox.showwarning("Advertencia", "Primero carga un grafo")
            return

        try:
            nodo_inicio = int(self.entry_nodo.get())
            profundidad = int(self.entry_profundidad.get())

            if nodo_inicio >= self.grafo.num_nodos:
                messagebox.showerror("Error", f"Nodo {nodo_inicio} no existe")
                return

            self._log(f"\n{'='*50}")

            if algoritmo == "bfs":
                resultado = self.grafo.bfs(nodo_inicio, profundidad)
                self._log(f"BFS desde nodo {nodo_inicio} (profundidad {profundidad}):")
                self._log(f"Nodos encontrados: {len(resultado)}")

                # Mostrar primeros 20
                self._log("\nPrimeros 20 nodos (nodo, nivel):")
                for nodo, nivel in resultado[:20]:
                    self._log(f"  Nivel {nivel}: Nodo {nodo}")

            elif algoritmo == "dfs":
                resultado = self.grafo.dfs(nodo_inicio, profundidad)
                self._log(f"DFS desde nodo {nodo_inicio} (profundidad {profundidad}):")
                self._log(f"Nodos encontrados: {len(resultado)}")

                # Mostrar primeros 20
                self._log("\nPrimeros 20 nodos visitados:")
                for i, nodo in enumerate(resultado[:20]):
                    self._log(f"  {i+1}. Nodo {nodo}")

        except ValueError:
            messagebox.showerror("Error", "Valores inválidos")

    def _camino_mas_corto(self):
        """Diálogo para encontrar camino más corto."""
        if not self.grafo or self.grafo.num_nodos == 0:
            messagebox.showwarning("Advertencia", "Primero carga un grafo")
            return

        # Crear diálogo
        dialog = tk.Toplevel(self.root)
        dialog.title("Camino más corto")
        dialog.geometry("300x150")
        dialog.transient(self.root)
        dialog.grab_set()

        ttk.Label(dialog, text="Nodo origen:").pack(pady=5)
        entry_origen = ttk.Entry(dialog)
        entry_origen.pack()
        entry_origen.insert(0, "0")

        ttk.Label(dialog, text="Nodo destino:").pack(pady=5)
        entry_destino = ttk.Entry(dialog)
        entry_destino.pack()
        entry_destino.insert(0, "100")

        def buscar():
            try:
                origen = int(entry_origen.get())
                destino = int(entry_destino.get())

                camino = self.grafo.camino_mas_corto(origen, destino)

                dialog.destroy()

                self._log(f"\n{'='*50}")
                self._log(f"Camino más corto de {origen} a {destino}:")

                if camino:
                    self._log(f"Longitud: {len(camino) - 1} aristas")
                    self._log(f"Camino: {' → '.join(map(str, camino))}")

                    # Visualizar el camino
                    aristas = [
                        (camino[i], camino[i + 1]) for i in range(len(camino) - 1)
                    ]
                    self._crear_pyvis(camino, aristas)
                else:
                    self._log("No existe camino entre los nodos")

            except ValueError:
                messagebox.showerror("Error", "Valores inválidos")

        ttk.Button(dialog, text="Buscar", command=buscar).pack(pady=10)

    def __del__(self):
        """Limpia archivos temporales."""
        try:
            if os.path.exists(self.html_file):
                os.remove(self.html_file)
            os.rmdir(self.temp_dir)
        except:
            pass


def main():
    """Punto de entrada principal."""
    root = tk.Tk()
    app = NeuroNetGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
