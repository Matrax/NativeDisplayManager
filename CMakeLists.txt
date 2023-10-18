#CMake version
cmake_minimum_required(VERSION 3.20)

# Options
option(BUILD_TESTS "Build the tests" OFF)

# Linux API
set(LINUX_API "XLIB")

# NativeDisplayManager library name and version
set(NDM_LIB_NAME "NativeDisplayManager")
set(NDM_LIB_VERSION 1.0)

# Includes directory
set(NDM_INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")

# Sources, libraries and tests files
if (WIN32)
    set(NDM_SOURCES "sources/win32_display_impl.cpp" "sources/win32_monitor_impl.cpp")
    set(NDM_LIBRARIES "opengl32.lib" "Gdi32.lib")
    set(NDM_TESTS "tests/win32_opengl_test.cpp")
elseif(LINUX)
    if(${LINUX_API} STREQUAL "XLIB")
        set(NDM_SOURCES "sources/xlib_display_impl.cpp" "sources/xlib_monitor_impl.cpp")
        set(NDM_LIBRARIES "X11")
        set(NDM_TESTS "tests/xlib_opengl_test.cpp")
    endif()
endif()

# Project
project(${NDM_LIB_NAME}
        VERSION ${NDM_LIB_VERSION}
        DESCRIPTION "Multi-platform display library"
        HOMEPAGE_URL "https://github.com/Matrax/NativeDisplayManager"
        LANGUAGES CXX)

# Create the library
add_library(${NDM_LIB_NAME} STATIC ${NDM_SOURCES})

# C++ properties
target_compile_features(NativeDisplayManager PUBLIC cxx_std_17)

# link libraries
target_link_libraries(${NDM_LIB_NAME} PUBLIC ${NDM_LIBRARIES})

# include directories
target_include_directories(${NDM_LIB_NAME} PUBLIC ${NDM_INCLUDES_DIR})

# Compile test
if(BUILD_TESTS)
    add_executable(ndm_test ${NDM_TESTS})
    target_link_libraries(ndm_test ${NDM_LIB_NAME})
endif()