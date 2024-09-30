#pragma once

// Windows only
#if defined(_WIN32) || defined(_WIN64)

// Win32 includes
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
    
#include <Windows.h>

namespace ndm
{
    // Win32 events process functions
    inline LRESULT CALLBACK win32_process_events(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

    // WGL function typedefs
    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
    typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

    // WGL function pointers
    inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

    // WGL constants
    constexpr int WGL_DRAW_TO_WINDOW_ARB = 0x2001;
    constexpr int WGL_SUPPORT_OPENGL_ARB = 0x2010;
    constexpr int WGL_DOUBLE_BUFFER_ARB = 0x2011;
    constexpr int WGL_ACCELERATION_ARB = 0x2003;
    constexpr int WGL_FULL_ACCELERATION_ARB = 0x2027;
    constexpr int WGL_PIXEL_TYPE_ARB = 0x2013;
    constexpr int WGL_TYPE_RGBA_ARB = 0x202B;
    constexpr int WGL_COLOR_BITS_ARB = 0x2014;
    constexpr int WGL_DEPTH_BITS_ARB = 0x2022;
    constexpr int WGL_ALPHA_BITS_ARB = 0x201B;
    constexpr int WGL_STENCIL_BITS_ARB = 0x2023;
    constexpr int WGL_SAMPLE_BUFFERS_ARB = 0x2041;
    constexpr int WGL_SAMPLES_ARB = 0x2042;
    constexpr int WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
    constexpr int WGL_CONTEXT_COMPABILITY_PROFILE_BIT_ARB = 0x00000002;
    constexpr int WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
    constexpr int WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;
    constexpr int WGL_CONTEXT_FLAGS_ARB = 0x2094;
    constexpr int WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;
}

#endif