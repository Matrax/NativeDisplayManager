#pragma once

// Windows only includes
#if defined(_WIN32) || defined(_WIN64)
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef UNICODE
	#define UNICODE
	#endif
#include <Windows.h>
#endif

// Windows only typedefs
#if defined(_WIN32) || defined(_WIN64)
    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
    typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

// Windows only global variables 
#if defined(_WIN32) || defined(_WIN64)
    inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
#endif

// Windows only functions
#if defined(_WIN32) || defined(_WIN64)
    inline BOOL CALLBACK Win32MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
    inline LRESULT CALLBACK Win32ProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
#endif

// Keyboard defines
#ifndef MAX_KEYBOARD_INPUTS
    #define MAX_KEYBOARD_INPUTS 10
#endif