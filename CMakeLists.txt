#CMake version
cmake_minimum_required(VERSION 3.20)

# Set CMake property
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# NativeDisplayManager variables
set(LIB_NAME "NativeDisplayManager")
set(LIB_VERSION 1.0)

# Build type
if(NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    message(STATUS "Set build type to debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")

# Project
project(${LIB_NAME}
        VERSION ${LIB_VERSION}
        DESCRIPTION "Multi-platform display library"
        HOMEPAGE_URL "https://github.com/Matrax/NativeDisplayManager"
        LANGUAGES CXX)

# If Windows
if (WIN32)

    add_library(${LIB_NAME} STATIC "sources/win32_display_impl.cpp")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_WIN32_EXECUTABLE False)
    endif()

    add_executable(win32_test "tests/win32_opengl_test.cpp")
    target_link_libraries(win32_test ${LIB_NAME} "opengl32.lib")
    target_include_directories(win32_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

endif (WIN32)