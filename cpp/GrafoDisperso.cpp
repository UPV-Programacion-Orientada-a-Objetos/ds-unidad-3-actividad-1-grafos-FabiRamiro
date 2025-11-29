/**
 * @file GrafoDisperso.cpp
 * @brief Implementación de la clase GrafoDisperso
 *
 * Implementación completa del grafo disperso usando formato CSR.
 * Incluye algoritmos BFS y DFS implementados "a mano".
 */

#include "GrafoDisperso.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_set>
#include <chrono>
#include <random>
#include <iomanip>

// ==================== CONSTRUCTOR Y DESTRUCTOR ====================

GrafoDisperso::GrafoDisperso() : numNodos(0), numAristas(0), maxNodoId(0)
{
    log("Inicializando GrafoDisperso...");
}

GrafoDisperso::~GrafoDisperso()
{
    limpiar();
}

// ==================== MÉTODOS PRIVADOS ====================

void GrafoDisperso::log(const std::string &mensaje) const
{
    std::cout << "[C++ Core] " << mensaje << std::endl;
}

void GrafoDisperso::construirCSR(std::vector<std::pair<size_t, size_t>> &aristas)
{
    log("Construyendo estructura CSR...");

    auto inicio = std::chrono::high_resolution_clock::now();

    // Encontrar el ID máximo de nodo
    maxNodoId = 0;
    for (const auto &arista : aristas)
    {
        maxNodoId = std::max(maxNodoId, std::max(arista.first, arista.second));
    }
    numNodos = maxNodoId + 1;
    numAristas = aristas.size();

    // Inicializar vectores
    row_ptr.resize(numNodos + 1, 0);
    col_indices.resize(numAristas);
    gradoEntrada.resize(numNodos, 0);

    // Paso 1: Contar aristas por nodo origen (grado de salida)
    for (const auto &arista : aristas)
    {
        row_ptr[arista.first + 1]++;
        gradoEntrada[arista.second]++;
    }

    // Paso 2: Acumular para obtener punteros de fila
    for (size_t i = 1; i <= numNodos; i++)
    {
        row_ptr[i] += row_ptr[i - 1];
    }

    // Paso 3: Llenar col_indices (usando copia de row_ptr como punteros de escritura)
    std::vector<size_t> write_ptr = row_ptr;
    for (const auto &arista : aristas)
    {
        size_t pos = write_ptr[arista.first]++;
        col_indices[pos] = arista.second;
    }

    // Paso 4: Ordenar vecinos de cada nodo (opcional, mejora búsqueda binaria)
    for (size_t i = 0; i < numNodos; i++)
    {
        std::sort(col_indices.begin() + row_ptr[i],
                  col_indices.begin() + row_ptr[i + 1]);
    }

    auto fin = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio);

    log("Estructura CSR construida. Tiempo: " + std::to_string(duracion.count()) + "ms");
}

// ==================== CARGA DE DATOS ====================

bool GrafoDisperso::cargarDatos(const std::string &filename)
{
    log("Cargando dataset '" + filename + "'...");

    auto inicio = std::chrono::high_resolution_clock::now();

    std::ifstream archivo(filename);
    if (!archivo.is_open())
    {
        log("ERROR: No se pudo abrir el archivo: " + filename);
        return false;
    }

    std::vector<std::pair<size_t, size_t>> aristas;
    std::string linea;
    size_t lineasProcesadas = 0;
    size_t lineasIgnoradas = 0;

    while (std::getline(archivo, linea))
    {
        // Ignorar líneas de comentarios (empiezan con #)
        if (linea.empty() || linea[0] == '#')
        {
            lineasIgnoradas++;
            continue;
        }

        std::istringstream iss(linea);
        size_t origen, destino;

        // Intentar leer como "origen destino" o "origen\tdestino"
        if (iss >> origen >> destino)
        {
            aristas.emplace_back(origen, destino);
            lineasProcesadas++;

            // Mostrar progreso cada 1 millón de aristas
            if (lineasProcesadas % 1000000 == 0)
            {
                log("Procesadas " + std::to_string(lineasProcesadas / 1000000) + "M aristas...");
            }
        }
        else
        {
            lineasIgnoradas++;
        }
    }

    archivo.close();

    if (aristas.empty())
    {
        log("ERROR: No se encontraron aristas válidas en el archivo.");
        return false;
    }

    log("Aristas leídas: " + std::to_string(aristas.size()) +
        " | Líneas ignoradas: " + std::to_string(lineasIgnoradas));

    // Construir la estructura CSR
    construirCSR(aristas);

    auto fin = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio);

    log("Carga completa. Nodos: " + std::to_string(numNodos) +
        " | Aristas: " + std::to_string(numAristas));
    log("Memoria estimada: " + std::to_string(getMemoriaUsada() / (1024 * 1024)) + " MB");
    log("Tiempo total de carga: " + std::to_string(duracion.count()) + "ms");

    return true;
}

// ==================== MÉTODOS DE CONSULTA ====================

size_t GrafoDisperso::getNumNodos() const
{
    return numNodos;
}

size_t GrafoDisperso::getNumAristas() const
{
    return numAristas;
}

size_t GrafoDisperso::getGradoSalida(size_t nodo) const
{
    if (nodo >= numNodos)
        return 0;
    return row_ptr[nodo + 1] - row_ptr[nodo];
}

size_t GrafoDisperso::getGradoEntrada(size_t nodo) const
{
    if (nodo >= numNodos)
        return 0;
    return gradoEntrada[nodo];
}

std::vector<size_t> GrafoDisperso::getVecinos(size_t nodo) const
{
    std::vector<size_t> vecinos;
    if (nodo >= numNodos)
        return vecinos;

    size_t inicio = row_ptr[nodo];
    size_t fin = row_ptr[nodo + 1];

    vecinos.reserve(fin - inicio);
    for (size_t i = inicio; i < fin; i++)
    {
        vecinos.push_back(col_indices[i]);
    }

    return vecinos;
}

bool GrafoDisperso::existeArista(size_t origen, size_t destino) const
{
    if (origen >= numNodos || destino >= numNodos)
        return false;

    size_t inicio = row_ptr[origen];
    size_t fin = row_ptr[origen + 1];

    // Búsqueda binaria (los vecinos están ordenados)
    return std::binary_search(col_indices.begin() + inicio,
                              col_indices.begin() + fin,
                              destino);
}

// ==================== ALGORITMOS DE BÚSQUEDA ====================

std::vector<std::pair<size_t, int>> GrafoDisperso::BFS(size_t inicio, int profundidadMax) const
{
    log("Ejecutando BFS desde nodo " + std::to_string(inicio) +
        " con profundidad máxima: " + (profundidadMax < 0 ? "sin límite" : std::to_string(profundidadMax)));

    auto tiempoInicio = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<size_t, int>> resultado;

    if (inicio >= numNodos)
    {
        log("ERROR: Nodo de inicio fuera de rango.");
        return resultado;
    }

    // Vector de visitados (usamos vector<bool> por eficiencia de memoria)
    std::vector<bool> visitado(numNodos, false);

    // Cola para BFS: almacena (nodo, nivel)
    std::queue<std::pair<size_t, int>> cola;

    // Iniciar desde el nodo de inicio
    cola.push({inicio, 0});
    visitado[inicio] = true;

    while (!cola.empty())
    {
        auto [nodoActual, nivel] = cola.front();
        cola.pop();

        // Agregar al resultado
        resultado.emplace_back(nodoActual, nivel);

        // Si alcanzamos la profundidad máxima, no expandir más
        if (profundidadMax >= 0 && nivel >= profundidadMax)
        {
            continue;
        }

        // Explorar vecinos
        size_t inicioVecinos = row_ptr[nodoActual];
        size_t finVecinos = row_ptr[nodoActual + 1];

        for (size_t i = inicioVecinos; i < finVecinos; i++)
        {
            size_t vecino = col_indices[i];
            if (!visitado[vecino])
            {
                visitado[vecino] = true;
                cola.push({vecino, nivel + 1});
            }
        }
    }

    auto tiempoFin = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(tiempoFin - tiempoInicio);

    log("BFS completado. Nodos encontrados: " + std::to_string(resultado.size()) +
        " | Tiempo: " + std::to_string(duracion.count() / 1000.0) + "ms");

    return resultado;
}

std::vector<size_t> GrafoDisperso::DFS(size_t inicio, int profundidadMax) const
{
    log("Ejecutando DFS desde nodo " + std::to_string(inicio) +
        " con profundidad máxima: " + (profundidadMax < 0 ? "sin límite" : std::to_string(profundidadMax)));

    auto tiempoInicio = std::chrono::high_resolution_clock::now();

    std::vector<size_t> resultado;

    if (inicio >= numNodos)
    {
        log("ERROR: Nodo de inicio fuera de rango.");
        return resultado;
    }

    // Vector de visitados
    std::vector<bool> visitado(numNodos, false);

    // Pila para DFS: almacena (nodo, nivel)
    std::stack<std::pair<size_t, int>> pila;

    // Iniciar desde el nodo de inicio
    pila.push({inicio, 0});

    while (!pila.empty())
    {
        auto [nodoActual, nivel] = pila.top();
        pila.pop();

        // Si ya fue visitado, saltar
        if (visitado[nodoActual])
        {
            continue;
        }

        // Marcar como visitado y agregar al resultado
        visitado[nodoActual] = true;
        resultado.push_back(nodoActual);

        // Si alcanzamos la profundidad máxima, no expandir más
        if (profundidadMax >= 0 && nivel >= profundidadMax)
        {
            continue;
        }

        // Explorar vecinos (en orden inverso para mantener orden natural)
        size_t inicioVecinos = row_ptr[nodoActual];
        size_t finVecinos = row_ptr[nodoActual + 1];

        for (size_t i = finVecinos; i > inicioVecinos; i--)
        {
            size_t vecino = col_indices[i - 1];
            if (!visitado[vecino])
            {
                pila.push({vecino, nivel + 1});
            }
        }
    }

    auto tiempoFin = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(tiempoFin - tiempoInicio);

    log("DFS completado. Nodos encontrados: " + std::to_string(resultado.size()) +
        " | Tiempo: " + std::to_string(duracion.count() / 1000.0) + "ms");

    return resultado;
}

std::vector<size_t> GrafoDisperso::caminoMasCorto(size_t origen, size_t destino) const
{
    log("Buscando camino más corto de " + std::to_string(origen) +
        " a " + std::to_string(destino));

    std::vector<size_t> camino;

    if (origen >= numNodos || destino >= numNodos)
    {
        log("ERROR: Nodos fuera de rango.");
        return camino;
    }

    if (origen == destino)
    {
        camino.push_back(origen);
        return camino;
    }

    // BFS con seguimiento de padres
    std::vector<int> padre(numNodos, -1);
    std::vector<bool> visitado(numNodos, false);
    std::queue<size_t> cola;

    cola.push(origen);
    visitado[origen] = true;
    bool encontrado = false;

    while (!cola.empty() && !encontrado)
    {
        size_t actual = cola.front();
        cola.pop();

        size_t inicioVecinos = row_ptr[actual];
        size_t finVecinos = row_ptr[actual + 1];

        for (size_t i = inicioVecinos; i < finVecinos && !encontrado; i++)
        {
            size_t vecino = col_indices[i];
            if (!visitado[vecino])
            {
                visitado[vecino] = true;
                padre[vecino] = static_cast<int>(actual);

                if (vecino == destino)
                {
                    encontrado = true;
                }
                else
                {
                    cola.push(vecino);
                }
            }
        }
    }

    if (!encontrado)
    {
        log("No existe camino entre los nodos.");
        return camino;
    }

    // Reconstruir camino
    size_t actual = destino;
    while (actual != origen)
    {
        camino.push_back(actual);
        actual = static_cast<size_t>(padre[actual]);
    }
    camino.push_back(origen);

    // Invertir para tener origen -> destino
    std::reverse(camino.begin(), camino.end());

    log("Camino encontrado con longitud: " + std::to_string(camino.size() - 1));

    return camino;
}

// ==================== ANÁLISIS ====================

std::pair<size_t, size_t> GrafoDisperso::getNodoMayorGrado() const
{
    log("Buscando nodo con mayor grado...");

    size_t mejorNodo = 0;
    size_t mejorGrado = 0;

    for (size_t i = 0; i < numNodos; i++)
    {
        size_t grado = getGradoSalida(i);
        if (grado > mejorGrado)
        {
            mejorGrado = grado;
            mejorNodo = i;
        }
    }

    log("Nodo con mayor grado: " + std::to_string(mejorNodo) +
        " con grado " + std::to_string(mejorGrado));

    return {mejorNodo, mejorGrado};
}

std::vector<std::pair<size_t, size_t>> GrafoDisperso::getTopKNodos(size_t k) const
{
    log("Obteniendo top " + std::to_string(k) + " nodos por grado...");

    // Crear vector de (nodo, grado)
    std::vector<std::pair<size_t, size_t>> nodosGrado;
    nodosGrado.reserve(numNodos);

    for (size_t i = 0; i < numNodos; i++)
    {
        size_t grado = getGradoSalida(i);
        if (grado > 0)
        { // Solo incluir nodos con aristas
            nodosGrado.emplace_back(i, grado);
        }
    }

    // Ordenar parcialmente (solo los k primeros)
    k = std::min(k, nodosGrado.size());
    std::partial_sort(nodosGrado.begin(), nodosGrado.begin() + k, nodosGrado.end(),
                      [](const auto &a, const auto &b)
                      { return a.second > b.second; });

    // Retornar solo los primeros k
    nodosGrado.resize(k);

    log("Top " + std::to_string(k) + " nodos obtenidos.");

    return nodosGrado;
}

std::vector<std::pair<size_t, size_t>> GrafoDisperso::getAristasSubgrafo(const std::vector<size_t> &nodos) const
{
    std::vector<std::pair<size_t, size_t>> aristas;

    // Crear set para búsqueda rápida
    std::unordered_set<size_t> nodosSet(nodos.begin(), nodos.end());

    // Para cada nodo en el subgrafo
    for (size_t nodo : nodos)
    {
        if (nodo >= numNodos)
            continue;

        // Obtener sus vecinos
        size_t inicio = row_ptr[nodo];
        size_t fin = row_ptr[nodo + 1];

        for (size_t i = inicio; i < fin; i++)
        {
            size_t vecino = col_indices[i];
            // Solo incluir arista si el vecino también está en el subgrafo
            if (nodosSet.count(vecino))
            {
                aristas.emplace_back(nodo, vecino);
            }
        }
    }

    return aristas;
}

// ==================== UTILIDADES ====================

size_t GrafoDisperso::getMemoriaUsada() const
{
    size_t memoria = 0;

    // row_ptr
    memoria += row_ptr.capacity() * sizeof(size_t);

    // col_indices
    memoria += col_indices.capacity() * sizeof(size_t);

    // gradoEntrada
    memoria += gradoEntrada.capacity() * sizeof(size_t);

    // Variables miembro
    memoria += sizeof(numNodos) + sizeof(numAristas) + sizeof(maxNodoId);

    return memoria;
}

void GrafoDisperso::limpiar()
{
    row_ptr.clear();
    row_ptr.shrink_to_fit();

    col_indices.clear();
    col_indices.shrink_to_fit();

    gradoEntrada.clear();
    gradoEntrada.shrink_to_fit();

    numNodos = 0;
    numAristas = 0;
    maxNodoId = 0;

    log("Grafo limpiado y memoria liberada.");
}

// ==================== MÉTODOS ADICIONALES ====================

std::vector<size_t> GrafoDisperso::muestreoAleatorio(size_t cantidad) const
{
    std::vector<size_t> muestra;

    if (cantidad >= numNodos)
    {
        // Retornar todos los nodos
        muestra.reserve(numNodos);
        for (size_t i = 0; i < numNodos; i++)
        {
            muestra.push_back(i);
        }
        return muestra;
    }

    // Generador aleatorio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, numNodos - 1);

    std::unordered_set<size_t> seleccionados;
    while (seleccionados.size() < cantidad)
    {
        seleccionados.insert(dist(gen));
    }

    muestra.assign(seleccionados.begin(), seleccionados.end());
    std::sort(muestra.begin(), muestra.end());

    return muestra;
}

std::vector<size_t> GrafoDisperso::getNodosEnRango(size_t inicio, size_t fin) const
{
    std::vector<size_t> nodos;

    fin = std::min(fin, numNodos);
    inicio = std::min(inicio, fin);

    nodos.reserve(fin - inicio);
    for (size_t i = inicio; i < fin; i++)
    {
        nodos.push_back(i);
    }

    return nodos;
}

std::string GrafoDisperso::getEstadisticas() const
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "=== Estadísticas del Grafo ===\n";
    ss << "Nodos: " << numNodos << "\n";
    ss << "Aristas: " << numAristas << "\n";
    ss << "Densidad: " << (numNodos > 0 ? (double)numAristas / ((double)numNodos * numNodos) * 100 : 0) << "%\n";
    ss << "Memoria usada: " << getMemoriaUsada() / (1024.0 * 1024.0) << " MB\n";

    if (numNodos > 0)
    {
        auto [nodoMax, gradoMax] = getNodoMayorGrado();
        ss << "Nodo con mayor grado: " << nodoMax << " (grado: " << gradoMax << ")\n";
    }

    return ss.str();
}
