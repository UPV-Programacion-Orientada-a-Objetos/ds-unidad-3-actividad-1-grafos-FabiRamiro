/**
 * @file GrafoBase.hpp
 * @brief Clase abstracta que define la interfaz para grafos
 *
 * Define los métodos virtuales puros que cualquier implementación
 * de grafo debe proporcionar (polimorfismo).
 */

#ifndef GRAFO_BASE_HPP
#define GRAFO_BASE_HPP

#include <vector>
#include <string>
#include <utility>

/**
 * @class GrafoBase
 * @brief Interfaz abstracta para grafos
 *
 * Esta clase define el contrato que deben cumplir todas las
 * implementaciones de grafos en el sistema NeuroNet.
 */
class GrafoBase
{
public:
    // Destructor virtual para polimorfismo correcto
    virtual ~GrafoBase() = default;

    // ==================== MÉTODOS DE CARGA ====================

    /**
     * @brief Carga un grafo desde un archivo de lista de aristas
     * @param filename Ruta al archivo (formato: origen destino por línea)
     * @return true si la carga fue exitosa
     */
    virtual bool cargarDatos(const std::string &filename) = 0;

    // ==================== MÉTODOS DE CONSULTA ====================

    /**
     * @brief Obtiene el número total de nodos
     * @return Número de nodos en el grafo
     */
    virtual size_t getNumNodos() const = 0;

    /**
     * @brief Obtiene el número total de aristas
     * @return Número de aristas en el grafo
     */
    virtual size_t getNumAristas() const = 0;

    /**
     * @brief Obtiene el grado de salida de un nodo
     * @param nodo ID del nodo
     * @return Grado de salida (número de aristas que salen del nodo)
     */
    virtual size_t getGradoSalida(size_t nodo) const = 0;

    /**
     * @brief Obtiene el grado de entrada de un nodo
     * @param nodo ID del nodo
     * @return Grado de entrada (número de aristas que llegan al nodo)
     */
    virtual size_t getGradoEntrada(size_t nodo) const = 0;

    /**
     * @brief Obtiene los vecinos (nodos destino) de un nodo
     * @param nodo ID del nodo origen
     * @return Vector con los IDs de los vecinos
     */
    virtual std::vector<size_t> getVecinos(size_t nodo) const = 0;

    /**
     * @brief Verifica si existe una arista entre dos nodos
     * @param origen ID del nodo origen
     * @param destino ID del nodo destino
     * @return true si existe la arista
     */
    virtual bool existeArista(size_t origen, size_t destino) const = 0;

    // ==================== ALGORITMOS DE BÚSQUEDA ====================

    /**
     * @brief Búsqueda en anchura (BFS) desde un nodo
     * @param inicio Nodo de inicio
     * @param profundidadMax Profundidad máxima a explorar (-1 = sin límite)
     * @return Vector de pares (nodo, nivel/distancia)
     */
    virtual std::vector<std::pair<size_t, int>> BFS(size_t inicio, int profundidadMax = -1) const = 0;

    /**
     * @brief Búsqueda en profundidad (DFS) desde un nodo
     * @param inicio Nodo de inicio
     * @param profundidadMax Profundidad máxima a explorar (-1 = sin límite)
     * @return Vector de nodos visitados en orden DFS
     */
    virtual std::vector<size_t> DFS(size_t inicio, int profundidadMax = -1) const = 0;

    /**
     * @brief Encuentra el camino más corto entre dos nodos usando BFS
     * @param origen Nodo de inicio
     * @param destino Nodo destino
     * @return Vector con el camino (vacío si no existe)
     */
    virtual std::vector<size_t> caminoMasCorto(size_t origen, size_t destino) const = 0;

    // ==================== ANÁLISIS ====================

    /**
     * @brief Obtiene el nodo con mayor grado de salida
     * @return Par (ID del nodo, grado)
     */
    virtual std::pair<size_t, size_t> getNodoMayorGrado() const = 0;

    /**
     * @brief Obtiene los K nodos con mayor grado
     * @param k Número de nodos a retornar
     * @return Vector de pares (ID del nodo, grado) ordenados descendentemente
     */
    virtual std::vector<std::pair<size_t, size_t>> getTopKNodos(size_t k) const = 0;

    /**
     * @brief Obtiene las aristas de un subgrafo (para visualización)
     * @param nodos Vector de nodos que forman el subgrafo
     * @return Vector de aristas (origen, destino)
     */
    virtual std::vector<std::pair<size_t, size_t>> getAristasSubgrafo(const std::vector<size_t> &nodos) const = 0;

    // ==================== UTILIDADES ====================

    /**
     * @brief Obtiene la memoria estimada usada por la estructura
     * @return Memoria en bytes
     */
    virtual size_t getMemoriaUsada() const = 0;

    /**
     * @brief Limpia el grafo y libera memoria
     */
    virtual void limpiar() = 0;
};

#endif // GRAFO_BASE_HPP
