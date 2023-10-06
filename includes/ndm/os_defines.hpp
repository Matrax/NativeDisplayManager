#pragma once

// Windows only
#if defined(_WIN32) || defined(_WIN64)

	#ifndef WIN32_LEAN_AND_MEAN
	    #define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef UNICODE
	    #define UNICODE
	#endif

    #include <Windows.h>

    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
    typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

    inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

    inline BOOL CALLBACK Win32MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
    inline LRESULT CALLBACK Win32ProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

    struct NativeAttributes
    {
        HWND handle;
        MSG messages;
        HDC device_context;
        HGLRC gl_device_context;
        HINSTANCE instance;
    };

#endif

#if defined(__linux__)

    #include <X11/Xlib.h>

    struct NativeAttributes
    {
        Display * display;
        Window window;
        XEvent events;
        int screen;
        unsigned long black_pixel;
        unsigned long white_pixel;
    };

#endif

#include <iostream>