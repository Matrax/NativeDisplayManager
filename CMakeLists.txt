#CMake version
cmake_minimum_required(VERSION 3.10)

# Project
project(NativeDisplayManager VERSION 1.0 LANGUAGES CXX)

# Set CMake property
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Executable
if (WIN32)
    add_library(NativeDisplayManager sources/win32_display_impl.cpp)
endif (WIN32)

if (UNIX)
    message([FATAL_ERROR] " OS not supported yet !")
endif (UNIX)

if (APPLE)
    message([FATAL_ERROR] " OS not supported yet !")
endif (APPLE)