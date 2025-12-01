from setuptools import setup, Extension
from Cython.Build import cythonize
import sys

extra_compile_args = ["-std=c++17"]
if sys.platform == "win32":
    extra_compile_args = ["/std:c++17"]

extensions = [
    Extension(
        name="neuronet_core",
        sources=["neuronet_core.pyx", "sparse_graph.cpp"],
        language="c++",
        extra_compile_args=extra_compile_args,
        include_dirs=["."],
    )
]

setup(
    name="neuronet_core",
    ext_modules=cythonize(
        extensions,
        compiler_directives={"language_level": "3"}
    ),
)
