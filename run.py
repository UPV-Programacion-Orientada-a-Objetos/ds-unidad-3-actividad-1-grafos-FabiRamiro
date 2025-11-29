#!/usr/bin/env python3
"""
run.py - Script para ejecutar NeuroNet

Uso:
    python run.py          # Inicia la GUI
    python run.py --build  # Compila el módulo C++ primero
    python run.py --test   # Ejecuta un test rápido
"""

import sys
import os
import subprocess

# Añadir el directorio raíz al path
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT_DIR)
sys.path.insert(0, os.path.join(ROOT_DIR, "python"))


def compilar():
    """Compila el módulo C++ con Cython."""
    print("=" * 60)
    print("Compilando módulo C++...")
    print("=" * 60)

    result = subprocess.run(
        [sys.executable, "setup.py", "build_ext", "--inplace"], cwd=ROOT_DIR
    )

    if result.returncode == 0:
        print("\nCompilación exitosa!")
        return True
    else:
        print("\nError en la compilación")
        return False


def test_rapido():
    """Ejecuta un test rápido del módulo."""
    print("=" * 60)
    print("Ejecutando test rápido...")
    print("=" * 60)

    try:
        from grafo_wrapper import PyGrafoDisperso

        grafo = PyGrafoDisperso()
        print("Módulo importado correctamente")

        # Buscar archivo de prueba
        test_file = os.path.join(ROOT_DIR, "Amazon0601.txt")
        if os.path.exists(test_file):
            print(f"\nCargando: {test_file}")
            if grafo.cargar_datos(test_file):
                print(f"\nEstadísticas:")
                print(f"  Nodos: {grafo.num_nodos:,}")
                print(f"  Aristas: {grafo.num_aristas:,}")
                print(f"  Memoria: {grafo.memoria_usada_mb:.2f} MB")

                nodo, grado = grafo.get_nodo_mayor_grado()
                print(f"  Nodo más conectado: {nodo} (grado: {grado})")

                # Test BFS
                print(f"\nTest BFS desde nodo 0, profundidad 2:")
                resultado = grafo.bfs(0, 2)
                print(f"  Nodos encontrados: {len(resultado)}")

                print("\Test completado!")
        else:
            print(f"\nNo se encontró archivo de prueba: {test_file}")
            print("  Descárgalo de: https://snap.stanford.edu/data/amazon0601.html")

    except ImportError as e:
        print(f"Error importando módulo: {e}")
        print("\nEjecuta primero: python run.py --build")


def iniciar_gui():
    """Inicia la interfaz gráfica."""
    try:
        from python.neuronet_gui import main

        main()
    except ImportError as e:
        print(f"Error: {e}")
        print("\nAsegúrate de haber compilado el módulo:")
        print("  python run.py --build")


def main():
    if len(sys.argv) > 1:
        arg = sys.argv[1].lower()

        if arg in ["--build", "-b", "build"]:
            compilar()
        elif arg in ["--test", "-t", "test"]:
            test_rapido()
        elif arg in ["--help", "-h", "help"]:
            print(__doc__)
        else:
            print(f"Argumento desconocido: {arg}")
            print("Usa --help para ver opciones")
    else:
        # Sin argumentos, iniciar GUI
        iniciar_gui()


if __name__ == "__main__":
    main()
