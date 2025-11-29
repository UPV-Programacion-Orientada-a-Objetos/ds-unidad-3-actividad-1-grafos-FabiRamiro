/**
 * @file GrafoDisperso.hpp
 * @brief Implementación de grafo usando formato CSR (Compressed Sparse Row)
 *
 * Esta clase implementa un grafo dirigido disperso optimizado para
 * manejar millones de nodos con bajo consumo de memoria.
 */

#ifndef GRAFO_DISPERSO_HPP
#define GRAFO_DISPERSO_HPP

#include "GrafoBase.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

/**
 * @class GrafoDisperso
 * @brief Implementación CSR de un grafo disperso
 *
 * Formato CSR (Compressed Sparse Row):
 * - row_ptr: Punteros al inicio de cada fila en col_indices
 * - col_indices: Índices de columnas (nodos destino)
 *
 * Para un grafo, row_ptr[i] indica dónde empiezan los vecinos del nodo i
 * en el array col_indices.
 *
 * Ejemplo:
 * Grafo: 0->1, 0->2, 1->2, 2->0
 * row_ptr:     [0, 2, 3, 4]
 * col_indices: [1, 2, 2, 0]
 *
 * Los vecinos del nodo 0 están en col_indices[row_ptr[0]:row_ptr[1]] = [1,2]
 * Los vecinos del nodo 1 están en col_indices[row_ptr[1]:row_ptr[2]] = [2]
 * Los vecinos del nodo 2 están en col_indices[row_ptr[2]:row_ptr[3]] = [0]
 */
class GrafoDisperso : public GrafoBase
{
private:
    // ==================== ESTRUCTURA CSR ====================
    std::vector<size_t> row_ptr;     // Punteros de fila (tamaño: numNodos + 1)
    std::vector<size_t> col_indices; // Índices de columna (tamaño: numAristas)

    // ==================== METADATOS ====================
    size_t numNodos;   // Número total de nodos
    size_t numAristas; // Número total de aristas
    size_t maxNodoId;  // ID máximo de nodo encontrado

    // Para calcular grado de entrada (opcional, usa más memoria)
    std::vector<size_t> gradoEntrada;

    // ==================== MÉTODOS PRIVADOS ====================

    /**
     * @brief Construye la estructura CSR desde una lista de aristas
     * @param aristas Vector de pares (origen, destino)
     */
    void construirCSR(std::vector<std::pair<size_t, size_t>> &aristas);

    /**
     * @brief Log helper para mensajes de consola
     */
    void log(const std::string &mensaje) const;

public:
    // ==================== CONSTRUCTORES ====================

    GrafoDisperso();
    ~GrafoDisperso() override;

    // ==================== IMPLEMENTACIÓN DE GRAFOBASE ====================

    bool cargarDatos(const std::string &filename) override;

    size_t getNumNodos() const override;
    size_t getNumAristas() const override;
    size_t getGradoSalida(size_t nodo) const override;
    size_t getGradoEntrada(size_t nodo) const override;
    std::vector<size_t> getVecinos(size_t nodo) const override;
    bool existeArista(size_t origen, size_t destino) const override;

    std::vector<std::pair<size_t, int>> BFS(size_t inicio, int profundidadMax = -1) const override;
    std::vector<size_t> DFS(size_t inicio, int profundidadMax = -1) const override;
    std::vector<size_t> caminoMasCorto(size_t origen, size_t destino) const override;

    std::pair<size_t, size_t> getNodoMayorGrado() const override;
    std::vector<std::pair<size_t, size_t>> getTopKNodos(size_t k) const override;
    std::vector<std::pair<size_t, size_t>> getAristasSubgrafo(const std::vector<size_t> &nodos) const override;

    size_t getMemoriaUsada() const override;
    void limpiar() override;

    // ==================== MÉTODOS ADICIONALES ====================

    /**
     * @brief Obtiene una muestra aleatoria de nodos
     * @param cantidad Número de nodos a muestrear
     * @return Vector con IDs de nodos aleatorios
     */
    std::vector<size_t> muestreoAleatorio(size_t cantidad) const;

    /**
     * @brief Obtiene nodos en un rango específico
     * @param inicio ID inicial
     * @param fin ID final
     * @return Vector con IDs de nodos en el rango
     */
    std::vector<size_t> getNodosEnRango(size_t inicio, size_t fin) const;

    /**
     * @brief Obtiene estadísticas del grafo
     * @return String con estadísticas formateadas
     */
    std::string getEstadisticas() const;
};

#endif // GRAFO_DISPERSO_HPP
