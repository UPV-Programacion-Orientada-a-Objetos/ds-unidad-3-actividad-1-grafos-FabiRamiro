# 🛠️ Guía de Compilación y Ejecución - NeuroNet

Esta guía explica paso a paso cómo compilar y ejecutar el proyecto NeuroNet en Windows usando MinGW.

---

## 📋 Requisitos Previos

### Software necesario:

- **Python 3.14** (o la versión que tengas instalada)
- **MinGW-w64** (MSYS2 recomendado)
- **Git Bash** o terminal compatible

### Verificar instalaciones:

```bash
# Verificar Python
python --version

# Verificar GCC (MinGW)
gcc --version
```

---

## 📦 Paso 1: Instalar Dependencias de Python

Abre una terminal en el directorio del proyecto y ejecuta:

```bash
pip install -r requirements.txt
```

Esto instalará:

- `cython` - Para compilar el wrapper C++/Python
- `numpy` - Requerido por Cython
- `pyvis` - Para visualización interactiva de grafos
- `networkx` - Dependencia de PyVis
- `matplotlib` - Visualización adicional

---

## 🔨 Paso 2: Limpiar Compilaciones Anteriores (Opcional)

Si ya intentaste compilar antes, limpia los archivos generados:

```bash
rm -rf build/
rm -f *.pyd
rm -f cython/*.cpp
rm -f cython/*.c
```

---

## ⚙️ Paso 3: Compilar con MinGW

Ejecuta el siguiente comando para compilar el módulo C++:

```bash
python setup.py build_ext --inplace --compiler=mingw32
```

### ¿Qué hace este comando?

- `build_ext` - Compila las extensiones C++
- `--inplace` - Coloca el archivo compilado en el directorio actual
- `--compiler=mingw32` - **Importante:** Fuerza el uso de MinGW en lugar de MSVC

### Salida esperada:

```
Compiling cython/grafo_wrapper.pyx because it changed.
[1/1] Cythonizing cython/grafo_wrapper.pyx
running build_ext
building 'grafo_wrapper' extension
g++ -shared ... cpp/GrafoDisperso.cpp ...
g++ -shared ... cython/grafo_wrapper.cpp ...
copying build\lib.win-amd64-cpython-314\grafo_wrapper.cp314-win_amd64.pyd ->
```

---

## 📁 Paso 4: Copiar el Archivo Compilado

El archivo `.pyd` se genera en la carpeta `build/`. Cópialo al directorio raíz:

```bash
cp build/lib.win-amd64-cpython-314/grafo_wrapper.cp314-win_amd64.pyd .
```

> **Nota:** El nombre del archivo puede variar según tu versión de Python. Ajusta el comando si es necesario.

---

## 📚 Paso 5: Copiar la DLL de MinGW

El módulo compilado necesita la librería `libwinpthread-1.dll` de MinGW. Cópiala al directorio del proyecto:

```bash
cp /c/msys64/mingw64/bin/libwinpthread-1.dll .
```

> **Nota:** Si tu instalación de MSYS2 está en otra ubicación, ajusta la ruta. Por ejemplo:
>
> - `C:\msys64\mingw64\bin\` (instalación por defecto)
> - `C:\mingw64\bin\` (instalación independiente)

---

## ✅ Paso 6: Verificar la Compilación

Ejecuta el test rápido para verificar que todo funciona:

```bash
python run.py --test
```

### Salida esperada:

```
============================================================
Ejecutando test rápido...
============================================================
[C++ Core] Inicializando GrafoDisperso...
[Cython] Wrapper PyGrafoDisperso inicializado.
✅ Módulo importado correctamente

Cargando: ...\Amazon0601.txt
[C++ Core] Cargando dataset '...'
[C++ Core] Procesadas 1M aristas...
[C++ Core] Procesadas 2M aristas...
[C++ Core] Procesadas 3M aristas...
[C++ Core] Carga completa. Nodos: 403,394 | Aristas: 3,387,388
[C++ Core] Memoria estimada: 31 MB

✅ Test completado!
```

---

## 🚀 Paso 7: Ejecutar la Aplicación

### Opción A: Interfaz Gráfica (GUI)

```bash
python run.py
```

### Opción B: Test rápido en consola

```bash
python run.py --test
```

### Opción C: Ver ayuda

```bash
python run.py --help
```

---

## 🎨 Uso de la Interfaz Gráfica

1. **Cargar Grafo**: Haz clic en "📂 Cargar Grafo" y selecciona `Amazon0601.txt`
2. **Esperar carga**: El grafo tiene ~400k nodos, tarda ~1 segundo
3. **Seleccionar modo de visualización**:
   - **BFS desde nodo**: Explora vecinos desde un nodo específico
   - **Top K nodos**: Muestra los K nodos más conectados
   - **Rango de nodos**: Visualiza un rango de IDs
   - **Muestra aleatoria**: Selección aleatoria de nodos
4. **Ajustar parámetros**: Nodo inicio, profundidad, máximo de nodos
5. **Visualizar**: Clic en "🔍 Visualizar Subgrafo"
6. **Interactuar**: Se abre el navegador con el grafo interactivo

---

## ❗ Solución de Problemas

### Error: "DLL load failed while importing grafo_wrapper"

**Causa:** Falta la DLL `libwinpthread-1.dll`

**Solución:**

```bash
cp /c/msys64/mingw64/bin/libwinpthread-1.dll .
```

---

### Error: "No module named 'grafo_wrapper'"

**Causa:** El archivo `.pyd` no está en el directorio correcto

**Solución:**

```bash
cp build/lib.win-amd64-cpython-314/*.pyd .
```

---

### Error: "error: command 'gcc' failed"

**Causa:** MinGW no está en el PATH

**Solución:** Asegúrate de que el directorio `bin` de MinGW está en tu PATH:

```bash
export PATH="/c/msys64/mingw64/bin:$PATH"
```

---

### Error: Compilación usa MSVC en lugar de MinGW

**Causa:** No se especificó el compilador

**Solución:** Usa el flag `--compiler=mingw32`:

```bash
python setup.py build_ext --inplace --compiler=mingw32
```

---

## 📝 Resumen de Comandos

```bash
# 1. Instalar dependencias
pip install -r requirements.txt

# 2. Compilar con MinGW
python setup.py build_ext --inplace --compiler=mingw32

# 3. Copiar archivo compilado
cp build/lib.win-amd64-cpython-314/grafo_wrapper.cp314-win_amd64.pyd .

# 4. Copiar DLL necesaria
cp /c/msys64/mingw64/bin/libwinpthread-1.dll .

# 5. Verificar
python run.py --test

# 6. Ejecutar GUI
python run.py
```

---

## 📊 Rendimiento Esperado

Con el dataset `Amazon0601.txt`:

| Métrica              | Valor      |
| -------------------- | ---------- |
| Nodos                | 403,394    |
| Aristas              | 3,387,388  |
| Tiempo de carga      | ~1 segundo |
| Memoria usada        | ~32 MB     |
| Tiempo BFS (prof. 2) | ~0.04 ms   |

---

**¡Listo!** 🎉 Ya puedes usar NeuroNet para analizar grafos masivos.
