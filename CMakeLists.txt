cmake_minimum_required(VERSION 3.6)
project(mpi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER "/usr/bin/mpicc")
set(CMAKE_CXX_COMPILER "/usr/bin/mpic++")

add_executable(mpi block.cpp utilities.h column.cpp row.cpp)
