#pragma once

// Windows only
#if defined(_WIN32) || defined(_WIN64)

    // Win32 includes
	#ifndef WIN32_LEAN_AND_MEAN
	    #define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef UNICODE
	    #define UNICODE
	#endif
    
    #include <Windows.h>

    // WGL functions typedef
    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
    typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

    // WGL function pointers
    inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

    // Win32 process
    inline BOOL CALLBACK Win32MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
    inline LRESULT CALLBACK Win32ProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

    // Win32 native display attributes
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

    // X11 includes
    #include <X11/Xlib.h>

    // X11 native display attributes
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