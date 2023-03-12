#pragma once

// Windows includes
#if defined(_WIN32) or defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <windowsx.h>
#endif

// Windows typedefs
#if defined(_WIN32) or defined(_WIN64)
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

// STD includes
#include <iostream>
#include <string>
#include <string_view>
#include <exception>

// NativeDisplayManager includes
#include "display_events.hpp"

namespace NativeDisplayManager
{
	class Display
	{

	private:

		// Global instance
		inline static Display * GLOBAL_INSTANCE = nullptr;

		// Common attributes
		DisplayEvents m_events = {};
		bool m_loaded = false;

		// Attributes on Windows
		#if defined(_WIN32) or defined(_WIN64)
		static LRESULT CALLBACK WindowProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
		inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
		inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
		inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
		HWND m_handle = nullptr;
		MSG m_messages = {};
		HDC m_device_context = nullptr;
		HGLRC m_gl_device_context = nullptr;
		HINSTANCE m_instance = nullptr;
		#endif

	public:

		// Constructor
		Display()
		{
			if (GLOBAL_INSTANCE != nullptr)
				throw std::runtime_error("Can only create one display per application !");

			// Set the global instance
			GLOBAL_INSTANCE = this;
		}

		// No copy constructor
		Display(Display&) = delete;
		Display(const Display&) = delete;

		// Destructor
		~Display()
		{
			if (m_loaded == true)
				Unload();

			GLOBAL_INSTANCE = nullptr;
		}

		// Methods
		void Load(const std::string_view title, const int width, const int height, const bool visible = false);
		void SwapFrontAndBack(const int swap_interval) const noexcept;
		void MakeOldOpenGLContext(const bool double_buffer, const int color_bits, const int depth_bits, const int stencil_bits);
		void MakeOpenGLContext(const int major_version, const int minor_version, const bool double_buffer, const int color_bits, const int alpha_bits, const int depth_bits, const int stencil_bits, const bool samples_buffers, const int samples);
		void DeleteOpenGLContext() const;
		void SetTitle(const std::string_view title);
		void SetCursorPosition(const unsigned long x, const unsigned long y);
		void SetCursorPositionToCenter();
		void SetCursorVisible(const bool visible);
		void Close() const noexcept;

		void Hide() const noexcept;
		void Show() const noexcept;
		void SetX(const int x);
		void SetY(const int y);
		void SetWidth(const int width);
		void SetHeight(const int height);
		int GetX() const;
		int GetY() const;
		int GetWidth() const;
		int GetHeight() const;
		void Unload();
		DisplayEvents& GetEvents() noexcept;

		// Shared methods

		bool IsKeyPressed(unsigned long virtual_key)
		{
			for (unsigned long i = 0; i < 32; i++)
			{
				if (m_events.m_keys_down[i] == virtual_key)
					return true;
			}

			return false;
		}

		bool IsKeyReleased(unsigned long virtual_key)
		{
			for (unsigned long i = 0; i < 32; i++)
			{
				if (m_events.m_keys_up[i] == virtual_key)
					return true;
			}

			return false;
		}

		inline bool IsLeftMouseButtonPressed() const noexcept { return m_events.left_mouse_pressed; }
		inline bool IsLeftMouseButtonReleased() const noexcept { return m_events.left_mouse_released; }
		inline bool IsRightMouseButtonPressed() const noexcept { return m_events.right_mouse_pressed; }
		inline bool IsRightMouseButtonReleased() const noexcept { return m_events.right_mouse_released; }
		inline bool IsMiddleMouseButtonPressed() const noexcept { return m_events.middle_mouse_pressed; }
		inline bool IsMiddleMouseButtonReleased() const noexcept { return m_events.middle_mouse_released; }
		inline int GetMouseDirectionX() const noexcept { return m_events.mouse_direction_x; }
		inline int GetMouseDirectionY() const noexcept { return m_events.mouse_direction_y; }
		inline int GetMouseX() const noexcept { return m_events.mouse_x; }
		inline int GetMouseY() const noexcept { return m_events.mouse_y; }

		// Shared functions
		inline static Display * GetInstance() { return Display::GLOBAL_INSTANCE; }

	};
}
