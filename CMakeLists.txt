#CMake version
cmake_minimum_required(VERSION 3.20)

# Set default cmake properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    message(STATUS "Set build type to debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Functions
function(COMPILE_LIBRARY sources)
    add_library(${PROJECT_NAME} STATIC ${sources})
endfunction(COMPILE_LIBRARY)

function(COMPILE_TEST exec_name sources libraries)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_WIN32_EXECUTABLE False)
    endif()
    add_executable(${exec_name} ${sources})
    target_link_libraries(${exec_name} ${PROJECT_NAME} ${libraries})
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
    set(LIBS "opengl32.lib")
    compile_library("sources/win32_display_impl.cpp")
    compile_test("win32_test" "tests/win32_opengl_test.cpp" ${LIBS})
endif (WIN32)