# SoundplanePython: a python binding for the Madrona Soundplane
# Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
# Distributed under the MIT license


from distutils.core import setup
from distutils.core import Extension
from Cython.Build import cythonize

setup(ext_modules = cythonize(Extension("soundplanepy", 

        sources=["soundplanepy.pyx", "soundplanepy.cpp"],
        libraries=["soundplane"], 
        extra_compile_args=["-std=c++11", "-O3", "-Wno-unknown-pragmas", "-mfloat-abi=hard", "-march=native", "-mfpu=neon"],
        language="c++"
    )))


      