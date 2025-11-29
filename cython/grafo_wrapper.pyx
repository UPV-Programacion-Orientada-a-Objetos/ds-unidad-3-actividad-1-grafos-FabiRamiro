# distutils: language = c++
# cython: language_level = 3
# cython: boundscheck = False
# cython: wraparound = False

"""
grafo_wrapper.pyx - Wrapper Cython para GrafoDisperso

Este módulo expone la clase C++ GrafoDisperso a Python,
convirtiendo tipos de datos automáticamente.
"""

from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.string cimport string
from libcpp cimport bool
from cython.operator cimport dereference as deref

import time

# Importar la declaración de la clase C++
cdef extern from "GrafoDisperso.hpp":
    cdef cppclass GrafoDisperso:
        GrafoDisperso() except +
        bool cargarDatos(const string& filename) except +
        size_t getNumNodos() const
        size_t getNumAristas() const
        size_t getGradoSalida(size_t nodo) const
        size_t getGradoEntrada(size_t nodo) const
        vector[size_t] getVecinos(size_t nodo) const
        bool existeArista(size_t origen, size_t destino) const
        vector[pair[size_t, int]] BFS(size_t inicio, int profundidadMax) const
        vector[size_t] DFS(size_t inicio, int profundidadMax) const
        vector[size_t] caminoMasCorto(size_t origen, size_t destino) const
        pair[size_t, size_t] getNodoMayorGrado() const
        vector[pair[size_t, size_t]] getTopKNodos(size_t k) const
        vector[pair[size_t, size_t]] getAristasSubgrafo(const vector[size_t]& nodos) const
        size_t getMemoriaUsada() const
        void limpiar()
        vector[size_t] muestreoAleatorio(size_t cantidad) const
        vector[size_t] getNodosEnRango(size_t inicio, size_t fin) const
        string getEstadisticas() const


cdef class PyGrafoDisperso:
    """
    Wrapper Python para la clase C++ GrafoDisperso.
    
    Esta clase permite usar el grafo disperso desde Python,
    manejando automáticamente la conversión de tipos.
    
    Ejemplo de uso:
        grafo = PyGrafoDisperso()
        grafo.cargar_datos("amazon0601.txt")
        print(f"Nodos: {grafo.num_nodos}")
        vecinos = grafo.get_vecinos(0)
    """
    cdef GrafoDisperso* _grafo
    cdef public float tiempo_carga
    
    def __cinit__(self):
        """Inicializa el grafo C++."""
        self._grafo = new GrafoDisperso()
        self.tiempo_carga = 0.0
        print("[Cython] Wrapper PyGrafoDisperso inicializado.")
    
    def __dealloc__(self):
        """Libera la memoria del grafo C++."""
        if self._grafo != NULL:
            del self._grafo
            print("[Cython] Wrapper PyGrafoDisperso destruido.")
    
    # ==================== CARGA DE DATOS ====================
    
    def cargar_datos(self, str filename) -> bool:
        """
        Carga un grafo desde un archivo de lista de aristas.
        
        Args:
            filename: Ruta al archivo (formato: origen destino por línea)
        
        Returns:
            True si la carga fue exitosa
        """
        print(f"[Cython] Solicitud de carga: {filename}")
        cdef bytes py_bytes = filename.encode('utf-8')
        cdef string cpp_string = py_bytes
        
        inicio = time.time()
        cdef bool resultado = self._grafo.cargarDatos(cpp_string)
        self.tiempo_carga = time.time() - inicio
        
        if resultado:
            print(f"[Cython] Carga completada en {self.tiempo_carga:.2f}s")
        else:
            print("[Cython] ERROR: La carga falló.")
        
        return resultado
    
    # ==================== PROPIEDADES ====================
    
    @property
    def num_nodos(self) -> int:
        """Número total de nodos en el grafo."""
        return self._grafo.getNumNodos()
    
    @property
    def num_aristas(self) -> int:
        """Número total de aristas en el grafo."""
        return self._grafo.getNumAristas()
    
    @property
    def memoria_usada(self) -> int:
        """Memoria usada por la estructura en bytes."""
        return self._grafo.getMemoriaUsada()
    
    @property
    def memoria_usada_mb(self) -> float:
        """Memoria usada por la estructura en MB."""
        return self._grafo.getMemoriaUsada() / (1024.0 * 1024.0)
    
    # ==================== MÉTODOS DE CONSULTA ====================
    
    def get_grado_salida(self, size_t nodo) -> int:
        """Obtiene el grado de salida de un nodo."""
        return self._grafo.getGradoSalida(nodo)
    
    def get_grado_entrada(self, size_t nodo) -> int:
        """Obtiene el grado de entrada de un nodo."""
        return self._grafo.getGradoEntrada(nodo)
    
    def get_vecinos(self, size_t nodo) -> list:
        """
        Obtiene los vecinos (nodos destino) de un nodo.
        
        Args:
            nodo: ID del nodo origen
        
        Returns:
            Lista con los IDs de los vecinos
        """
        cdef vector[size_t] vecinos_cpp = self._grafo.getVecinos(nodo)
        return list(vecinos_cpp)
    
    def existe_arista(self, size_t origen, size_t destino) -> bool:
        """Verifica si existe una arista entre dos nodos."""
        return self._grafo.existeArista(origen, destino)
    
    # ==================== ALGORITMOS DE BÚSQUEDA ====================
    
    def bfs(self, size_t inicio, int profundidad_max=-1) -> list:
        """
        Búsqueda en anchura (BFS) desde un nodo.
        
        Args:
            inicio: Nodo de inicio
            profundidad_max: Profundidad máxima (-1 = sin límite)
        
        Returns:
            Lista de tuplas (nodo, nivel/distancia)
        """
        print(f"[Cython] Solicitud BFS desde nodo {inicio}, profundidad {profundidad_max}")
        cdef vector[pair[size_t, int]] resultado_cpp = self._grafo.BFS(inicio, profundidad_max)
        
        # Convertir a lista de tuplas Python
        resultado = [(p.first, p.second) for p in resultado_cpp]
        print(f"[Cython] BFS retornó {len(resultado)} nodos.")
        return resultado
    
    def dfs(self, size_t inicio, int profundidad_max=-1) -> list:
        """
        Búsqueda en profundidad (DFS) desde un nodo.
        
        Args:
            inicio: Nodo de inicio
            profundidad_max: Profundidad máxima (-1 = sin límite)
        
        Returns:
            Lista de nodos visitados en orden DFS
        """
        print(f"[Cython] Solicitud DFS desde nodo {inicio}, profundidad {profundidad_max}")
        cdef vector[size_t] resultado_cpp = self._grafo.DFS(inicio, profundidad_max)
        resultado = list(resultado_cpp)
        print(f"[Cython] DFS retornó {len(resultado)} nodos.")
        return resultado
    
    def camino_mas_corto(self, size_t origen, size_t destino) -> list:
        """
        Encuentra el camino más corto entre dos nodos.
        
        Args:
            origen: Nodo de inicio
            destino: Nodo destino
        
        Returns:
            Lista con el camino (vacía si no existe)
        """
        print(f"[Cython] Buscando camino de {origen} a {destino}")
        cdef vector[size_t] camino_cpp = self._grafo.caminoMasCorto(origen, destino)
        return list(camino_cpp)
    
    # ==================== ANÁLISIS ====================
    
    def get_nodo_mayor_grado(self) -> tuple:
        """
        Obtiene el nodo con mayor grado de salida.
        
        Returns:
            Tupla (ID del nodo, grado)
        """
        cdef pair[size_t, size_t] resultado = self._grafo.getNodoMayorGrado()
        return (resultado.first, resultado.second)
    
    def get_top_k_nodos(self, size_t k) -> list:
        """
        Obtiene los K nodos con mayor grado.
        
        Args:
            k: Número de nodos a retornar
        
        Returns:
            Lista de tuplas (ID del nodo, grado) ordenadas descendentemente
        """
        cdef vector[pair[size_t, size_t]] resultado_cpp = self._grafo.getTopKNodos(k)
        return [(p.first, p.second) for p in resultado_cpp]
    
    def get_aristas_subgrafo(self, list nodos) -> list:
        """
        Obtiene las aristas de un subgrafo.
        
        Args:
            nodos: Lista de nodos que forman el subgrafo
        
        Returns:
            Lista de tuplas (origen, destino)
        """
        cdef vector[size_t] nodos_cpp = nodos
        cdef vector[pair[size_t, size_t]] aristas_cpp = self._grafo.getAristasSubgrafo(nodos_cpp)
        return [(p.first, p.second) for p in aristas_cpp]
    
    # ==================== UTILIDADES ====================
    
    def limpiar(self):
        """Limpia el grafo y libera memoria."""
        self._grafo.limpiar()
        print("[Cython] Grafo limpiado.")
    
    def muestreo_aleatorio(self, size_t cantidad) -> list:
        """
        Obtiene una muestra aleatoria de nodos.
        
        Args:
            cantidad: Número de nodos a muestrear
        
        Returns:
            Lista con IDs de nodos aleatorios
        """
        cdef vector[size_t] muestra_cpp = self._grafo.muestreoAleatorio(cantidad)
        return list(muestra_cpp)
    
    def get_nodos_en_rango(self, size_t inicio, size_t fin) -> list:
        """
        Obtiene nodos en un rango específico.
        
        Args:
            inicio: ID inicial
            fin: ID final
        
        Returns:
            Lista con IDs de nodos en el rango
        """
        cdef vector[size_t] nodos_cpp = self._grafo.getNodosEnRango(inicio, fin)
        return list(nodos_cpp)
    
    def get_estadisticas(self) -> str:
        """Obtiene estadísticas formateadas del grafo."""
        cdef string stats_cpp = self._grafo.getEstadisticas()
        return stats_cpp.decode('utf-8')
    
    # ==================== MÉTODOS DE VISUALIZACIÓN ====================
    
    def get_subgrafo_bfs(self, size_t inicio, int profundidad=2) -> tuple:
        """
        Obtiene un subgrafo mediante BFS para visualización.
        
        Args:
            inicio: Nodo de inicio
            profundidad: Profundidad máxima
        
        Returns:
            Tupla (lista_nodos, lista_aristas)
        """
        # Obtener nodos mediante BFS
        nodos_con_nivel = self.bfs(inicio, profundidad)
        nodos = [n[0] for n in nodos_con_nivel]
        
        # Obtener aristas del subgrafo
        aristas = self.get_aristas_subgrafo(nodos)
        
        return (nodos, aristas)
    
    def get_subgrafo_top_k(self, size_t k) -> tuple:
        """
        Obtiene un subgrafo con los K nodos más importantes.
        
        Args:
            k: Número de nodos top
        
        Returns:
            Tupla (lista_nodos, lista_aristas)
        """
        top_k = self.get_top_k_nodos(k)
        nodos = [n[0] for n in top_k]
        aristas = self.get_aristas_subgrafo(nodos)
        
        return (nodos, aristas)
