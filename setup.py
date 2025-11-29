from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy as np
import os
import sys

# Configuración específica para Windows con MinGW o Linux/Mac
# MinGW usa flags estilo GCC, no MSVC
if sys.platform == "win32":
    # Flags para MinGW (GCC en Windows)
    extra_compile_args = ["-std=c++17", "-O3", "-Wall"]
    extra_link_args = ["-static-libgcc", "-static-libstdc++"]
else:
    # Flags para GCC/Clang en Linux/Mac
    extra_compile_args = ["-std=c++17", "-O3"]
    extra_link_args = []

# Definir la extensión de Cython
extensions = [
    Extension(
        name="grafo_wrapper",
        sources=["cython/grafo_wrapper.pyx", "cpp/GrafoDisperso.cpp"],
        include_dirs=["cpp", np.get_include()],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )
]

setup(
    name="NeuroNet",
    version="1.0.0",
    description="Sistema híbrido de análisis y visualización de redes masivas",
    author="UPV - Estructura de Datos",
    ext_modules=cythonize(
        extensions,
        compiler_directives={
            "language_level": "3",
            "embedsignature": True,
        },
    ),
    zip_safe=False,
)
