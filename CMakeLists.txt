#CMake version
cmake_minimum_required(VERSION 3.20)

# Options
option(BUILD_TESTS "Build the tests" OFF)
set(LINUX_API "XLIB")

# Sources, libraries and tests files
if (WIN32)
    set(NDM_SOURCES "sources/win32_display_impl.cpp" "sources/win32_monitor_impl.cpp")
    set(NDM_LIBRARIES "opengl32.lib" "Gdi32.lib")
    set(NDM_DISPLAY_TEST "tests/win32_display_test.cpp")
    set(NDM_MONITOR_TEST "tests/win32_monitor_test.cpp")
    set(NDM_OPENGL_TEST "tests/win32_opengl_test.cpp")
elseif(LINUX)
    if(${LINUX_API} STREQUAL "XLIB")
        set(NDM_SOURCES "sources/xlib_display_impl.cpp" "sources/xlib_monitor_impl.cpp")
        set(NDM_LIBRARIES "X11")
        set(NDM_DISPLAY_TEST "tests/xlib_opengl_test.cpp")
    endif()
endif()

# Project
project(NativeDisplayManager
        VERSION 1.0
        DESCRIPTION "Multi-platform display library"
        HOMEPAGE_URL "https://github.com/Matrax/NativeDisplayManager"
        LANGUAGES CXX)

# Create the library
add_library(NativeDisplayManager STATIC ${NDM_SOURCES})

# C++ properties
target_compile_features(NativeDisplayManager PUBLIC cxx_std_17)

# link libraries
target_link_libraries(NativeDisplayManager PUBLIC ${NDM_LIBRARIES})

# include directories
target_include_directories(NativeDisplayManager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/includes")

# Compile test
if(BUILD_TESTS)

    # Display test
    add_executable(ndm_display_test ${NDM_DISPLAY_TEST})
    target_link_libraries(ndm_display_test NativeDisplayManager)

    # Monitor test
    add_executable(ndm_monitor_test ${NDM_MONITOR_TEST})
    target_link_libraries(ndm_monitor_test NativeDisplayManager)

    # OpenGL test
    add_executable(ndm_opengl_test ${NDM_OPENGL_TEST})
    target_link_libraries(ndm_opengl_test NativeDisplayManager)

endif()