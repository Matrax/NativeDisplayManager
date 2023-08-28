#CMake version
cmake_minimum_required(VERSION 3.20)

# options
option(BUILD_TESTS "Build the tests" OFF)

# Set CMake properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# NativeDisplayManager variables
set(NDM_LIB_NAME "NativeDisplayManager")
set(NDM_LIB_VERSION 1.0)
set(NDM_INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")
set(NDM_WIN32_SOURCES "sources/win32_display_impl.cpp" 
                      "sources/win32_monitor_impl.cpp")
set(NDM_WIN32_LIBRARIES "opengl32.lib")
set(NDM_WIN32_TESTS "tests/win32_opengl_test.cpp")


# Project
project(${NDM_LIB_NAME}
        VERSION ${NDM_LIB_VERSION}
        DESCRIPTION "Multi-platform display library"
        HOMEPAGE_URL "https://github.com/Matrax/NativeDisplayManager"
        LANGUAGES CXX)

# If Windows
if (WIN32)

    # Create Win32 library
    add_library(${NDM_LIB_NAME} STATIC ${NDM_WIN32_SOURCES})
    target_link_libraries(${NDM_LIB_NAME} PUBLIC ${NDM_WIN32_LIBRARIES})
    target_include_directories(${NDM_LIB_NAME} PUBLIC ${NDM_INCLUDES_DIR})

    # Compile test
    if(BUILD_TESTS)
        add_executable(ndm_win32_test ${NDM_WIN32_TESTS})
        target_link_libraries(ndm_win32_test ${NDM_LIB_NAME})
    endif()

endif (WIN32)