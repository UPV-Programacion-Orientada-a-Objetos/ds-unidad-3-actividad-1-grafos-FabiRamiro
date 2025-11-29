# distutils: language = c++
# cython: language_level = 3

"""
grafo_wrapper.pxd - Declaraciones Cython para la clase GrafoDisperso de C++

Este archivo declara la interfaz C++ para que Cython pueda generar
el código de enlace apropiado.
"""

from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.string cimport string
from libcpp cimport bool

# Declaración de la clase C++
cdef extern from "GrafoDisperso.hpp":
    cdef cppclass GrafoDisperso:
        # Constructor y destructor
        GrafoDisperso() except +
        
        # Carga de datos
        bool cargarDatos(const string& filename) except +
        
        # Métodos de consulta
        size_t getNumNodos() const
        size_t getNumAristas() const
        size_t getGradoSalida(size_t nodo) const
        size_t getGradoEntrada(size_t nodo) const
        vector[size_t] getVecinos(size_t nodo) const
        bool existeArista(size_t origen, size_t destino) const
        
        # Algoritmos de búsqueda
        vector[pair[size_t, int]] BFS(size_t inicio, int profundidadMax) const
        vector[size_t] DFS(size_t inicio, int profundidadMax) const
        vector[size_t] caminoMasCorto(size_t origen, size_t destino) const
        
        # Análisis
        pair[size_t, size_t] getNodoMayorGrado() const
        vector[pair[size_t, size_t]] getTopKNodos(size_t k) const
        vector[pair[size_t, size_t]] getAristasSubgrafo(const vector[size_t]& nodos) const
        
        # Utilidades
        size_t getMemoriaUsada() const
        void limpiar()
        
        # Métodos adicionales
        vector[size_t] muestreoAleatorio(size_t cantidad) const
        vector[size_t] getNodosEnRango(size_t inicio, size_t fin) const
        string getEstadisticas() const
