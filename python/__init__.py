"""
NeuroNet - Sistema híbrido de análisis y visualización de redes masivas

Este paquete proporciona herramientas para analizar grafos masivos
utilizando un backend C++ con formato CSR y una interfaz Python.
"""

__version__ = "1.0.0"
__author__ = "Osvaldo Fabian Ramiro Balboa"

# Intentar importar el wrapper compilado
try:
    from grafo_wrapper import PyGrafoDisperso

    __all__ = ["PyGrafoDisperso"]
except ImportError:
    print(
        "ADVERTENCIA: Módulo C++ no compilado. Ejecuta: python setup.py build_ext --inplace"
    )
    __all__ = []
