#CMake version
cmake_minimum_required(VERSION 3.10)

# Project
project(NativeDisplayManager VERSION 1.0 LANGUAGES CXX)

# Set CMake property
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# If windows
if (WIN32)
    # Add static library and test executable
    add_library(NativeDisplayManager STATIC sources/win32_display_impl.cpp)
    add_executable(NativeDisplayManagerTest sources/win32_display_impl.cpp tests/win32_opengl_test.cpp)
    # Set static lib and include directory for the test executable
    target_link_libraries(NativeDisplayManagerTest opengl32.lib NativeDisplayManager)
    target_include_directories(NativeDisplayManagerTest PUBLIC includes)
endif (WIN32)

if (UNIX)
    message([FATAL_ERROR] " OS not supported yet !")
endif (UNIX)

if (APPLE)
    message([FATAL_ERROR] " OS not supported yet !")
endif (APPLE)