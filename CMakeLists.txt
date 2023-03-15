#CMake version
cmake_minimum_required(VERSION 3.20)

# Set default cmake properties and variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LIB_NAME "NativeDisplayManager")
set(WIN32_IMPLEMENTATION_FILE "sources/win32_display_impl.cpp")
set(WIN32_TEST_FILE "tests/win32_opengl_test.cpp")
set(WIN32_LINKER_LIBS "opengl32.lib")

if(NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    message(STATUS "Set build type to debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Functions
function(COMPILE_LIBRARY lib_name sources)
    add_library(${lib_name} STATIC ${sources})
endfunction(COMPILE_LIBRARY)

function(COMPILE_TEST lib_name exec_name sources libraries)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_WIN32_EXECUTABLE False)
    endif()
    add_executable(${exec_name} ${sources})
    target_link_libraries(${exec_name} ${lib_name} ${libraries})
    target_include_directories(${exec_name} PUBLIC "includes")
endfunction(COMPILE_TEST)

# Project
project(NativeDisplayManager 
        VERSION 
            1.0 
        DESCRIPTION
            "Multi-platform display library"
        HOMEPAGE_URL
            "https://github.com/Matrax/NativeDisplayManager"
        LANGUAGES 
            CXX)
message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")

# If Windows
if (WIN32)
    compile_library(${LIB_NAME} ${WIN32_IMPLEMENTATION_FILE})
    compile_test(${LIB_NAME} "win32_test" ${WIN32_TEST_FILE} ${WIN32_LINKER_LIBS})
endif (WIN32)