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
#include <windowsx.h>
#endif

// Windows only typedefs
#if defined(_WIN32) || defined(_WIN64)
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

// STD includes
#include <iostream>
#include <string>
#include <string_view>
#include <exception>
#include <vector>

namespace NativeDisplayManager
{
	struct MonitorInfo
	{
        std::string name;
		unsigned int width;
        unsigned int height;
	};
	
	/*
	* This structure represent all the events happening during the execution by a display, getters for all 
	* of them are defined in the display class directly, all implementations must use this structure to report 
	* all the following events :
	* - mouse and keyboard inputs 
	* - mouse position and mouse direction
	* - display resized, minimized, maximized, moved
	* - display closed
	* - os languages changed
	*/
	struct DisplayEvents
	{
		int m_keys_down[32];
		int m_keys_up[32];
		int previous_mouse_x;
		int previous_mouse_y;
		int mouse_x;
		int mouse_y;
		int mouse_direction_x;
		int mouse_direction_y;
		bool resized;
		bool closed;
		bool minimized;
		bool maximized;
		bool moved;
		bool moused_moved;
		bool language_changed;
		bool left_mouse_pressed;
		bool right_mouse_pressed;
		bool middle_mouse_pressed;
		bool left_mouse_released;
		bool right_mouse_released;
		bool middle_mouse_released;
	};

	/*
	* This class is a singleton that represent a display. There is no default implementation of this class, 
	* some methods need to be defined in an other class for each systems, also, some methods are shared by all the OS 
	* and are already defined in this header.
	* Each implementation can set attributes or methods in this class but can be only compiled with these, also this library is designed to report 
	* error mainly with exceptions.
	* Here the list of all implementation :
	* - win32_display_impl.cpp -> implements the Win32 (x32 or x64) version. 
	*/
	class Display
	{

	private:

		// Global instance
		inline static Display * GLOBAL_INSTANCE = nullptr;

		// Static
		inline static std::vector<MonitorInfo> monitors;

		// Shared attributes
		DisplayEvents m_events = {};
		bool m_loaded = false;

		// Windows only attributes
		#if defined(_WIN32) || defined(_WIN64)
		HWND m_handle = nullptr;
		MSG m_messages = {};
		HDC m_device_context = nullptr;
		HGLRC m_gl_device_context = nullptr;
		HINSTANCE m_instance = nullptr;
		#endif

		// Windows only static attributes 
		#if defined(_WIN32) || defined(_WIN64)
		inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
		inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
		inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
		#endif

		// Windows only static functions
		#if defined(_WIN32) || defined(_WIN64)
		static LRESULT CALLBACK WindowProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
		#endif

	public:

		/*
		* Constructor of this class.
		* This class is a singleton so if a display already exist, an exception is thrown. If not, the global instance is set.
		*/
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

		/*
		* Destructor of this class.
		* If the display is loaded, the destructor unload the display, and then release the global instance.
		*/
		~Display()
		{
			if (m_loaded == true)
				Unload();

			GLOBAL_INSTANCE = nullptr;
		}

		/*
		* This static function get the global instance.
		* @return The global instance.
		*/
		inline static Display * GetInstance() { return Display::GLOBAL_INSTANCE; }

		/*
		* This static function get informations about a monitor specified with the id.
		* This static function need to be implemented for each OS.
		* @return All informations about a monitor specified with the id.
		*/
		static std::vector<MonitorInfo> GetMonitors(); 

		/*
		* This methods must return all the events catched by the display.
		* This methods need to be implemented for each OS.
		* @return DisplayEvents& The events structure
		*/
		DisplayEvents& GetEvents() noexcept;

		/*
		* This methods load the display.
		* This methods need to be implemented for each OS.
		* @param title The title of the display.
		* @param width The width of the display on the screen.
		* @param height The height of the display on the screen.
		*/
		void Load(const std::string_view title, const int width, const int height, const bool visible = false);

		/*
		* This methods unload the display by releasing all the associated resources.
		* This methods need to be implemented for each OS.
		*/
		void Unload();

		/*
		* This methods set the display on full screen mode or not.
		* This methods need to be implemented for each OS.
		* @param fullscreen If the full screen mode is activated or not.
		*/
		void SetFullScreen(const bool fullscreen);

		/*
		* This methods swap the back and front buffer of the display if the display used double buffering.
		* This methods need to be implemented for each OS.
		* @param fullscreen The time interval between two swap (0 = no vsync, 1 = vsync).
		*/
		void SwapFrontAndBack(const int swap_interval) const noexcept;

		/*
		* This methods create an OpenGL context in an old way.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This methods need to be implemented for each OS.
		* @param double_buffer Use double buffering.
		* @param color_bits Number of bits to represent a color on the color buffer.
		* @param depth_bits Number of bits to represent a data on the depth buffer.
		* @param depth_bits Number of bits to represent a data on the stencil buffer.
		*/
		void MakeOldOpenGLContext(const bool double_buffer, const int color_bits, const int depth_bits, const int stencil_bits);

		/*
		* This methods create an OpenGL context.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This methods need to be implemented for each OS.
		* @param major_version The OpenGL context major version to load.
		* @param minor_version The OpenGL context minor version to load.
		* @param double_buffer Use double buffering.
		* @param color_bits Number of bits to represent a color on the color buffer.
		* @param alpha_bits Number of bits to represent the alpha channel on the color buffer.
		* @param depth_bits Number of bits to represent a data on the depth buffer.
		* @param stencil_bits Number of bits to represent a data on the stencil buffer.
		* @param samples_buffers Use samples in the buffers.
		* @param samples Number of samples.
		*/
		void MakeOpenGLContext(const int major_version, const int minor_version, const bool double_buffer, const int color_bits, 
							   const int alpha_bits, const int depth_bits, const int stencil_bits, const bool samples_buffers, const int samples);
		
		/*
		* This methods delete the current OpenGL context.
		* This methods need to be implemented for each OS.
		*/
		void DeleteOpenGLContext() const;

		/*
		* This methods set the title of the display.
		* This methods need to be implemented for each OS.
		* @param title The new title of the display.
		*/
		void SetTitle(const std::string_view title);

		/*
		* This methods set the cursor position on the display.
		* This methods need to be implemented for each OS.
		* @param x The x position of the cursor on the screen.
		* @param y The y position of the cursor on the screen.
		*/
		void SetCursorPosition(const unsigned long x, const unsigned long y);

		/*
		* This methods set the cursor position on the center of the display.
		* This methods need to be implemented for each OS.
		*/
		void SetCursorPositionToCenter();

		/*
		* This methods set the visibility of the cursor on the display.
		* This methods need to be implemented for each OS.
		* @param visible The visibility of the cursor.
		*/
		void SetCursorVisible(const bool visible);

		/*
		* This methods set the visibility of the display.
		* This methods need to be implemented for each OS.
		* @param visible The visibility of the display.
		*/
		void SetVisible(const bool visible);

		/*
		* This methods destroy the display on the screen.
		* This methods need to be implemented for each OS.
		*/
		void Close() const;

		/*
		* This methods set the display x position on the screen.
		* This methods need to be implemented for each OS.
		* @param x The x position
		*/
		void SetX(const int x);

		/*
		* This methods set the display y position on the screen.
		* This methods need to be implemented for each OS.
		* @param y The y display
		*/
		void SetY(const int y);

		/*
		* This methods set the display width.
		* This methods need to be implemented for each OS.
		* @param width The width of the display
		*/
		void SetWidth(const int width);

		/*
		* This methods set the display height.
		* This methods need to be implemented for each OS.
		* @param height The height of the display
		*/
		void SetHeight(const int height);

		/*
		* This methods check if the display has the focus (if the display is on the top).
		* This methods need to be implemented for each OS.
		* @return If the window has the focus.
		*/
		bool HasFocus() const;

		/*
		* This methods get the x position of the display on the screen.
		* This methods need to be implemented for each OS.
		* @return The x position of the display on the screen.
		*/
		int GetX() const;

		/*
		* This methods get the y position of the display on the screen.
		* This methods need to be implemented for each OS.
		* @return The y position of the display on the screen.
		*/
		int GetY() const;

		/*
		* This methods get the width of the display.
		* This methods need to be implemented for each OS.
		* @return The width position of the display.
		*/
		int GetWidth() const;

		/*
		* This methods get the height of the display.
		* This methods need to be implemented for each OS.
		* @return The height position of the display.
		*/
		int GetHeight() const;

		/*
		* This methods check if a virtual key is pressed.
		* @param virtual_key The virtual key to check.
		*/
		bool IsKeyPressed(int virtual_key) const noexcept
		{
			for (size_t i = 0; i < 32; i++)
			{
				if (m_events.m_keys_down[i] == virtual_key)
					return true;
			}

			return false;
		}

		/*
		* This methods check if a virtual key is released.
		* @param virtual_key The virtual key to check.
		*/
		bool IsKeyReleased(int virtual_key) const noexcept
		{
			for (size_t i = 0; i < 32; i++)
			{
				if (m_events.m_keys_up[i] == virtual_key)
					return true;
			}

			return false;
		}

		/*
		* This methods add a key pressed in the events struct, it also removed the same key from the keys released.
		* @param virtual_key The virtual key to add.
		*/
		void AddKeyPressed(int virtual_key)
		{
			bool already_pressed = false;

			// Check if the key is already in the array
			for (size_t i = 0; i < 32 && already_pressed == false; i++)
			{
				if (m_events.m_keys_down[i] == virtual_key)
					already_pressed = true;
			}

			// If not, we add the key and remove from the keys up array
			if (already_pressed == false)
			{
				for (size_t i = 0; i < 32; i++)
				{
					if (m_events.m_keys_down[i] == 0)
					{
						m_events.m_keys_down[i] = virtual_key;
						break;
					}
				}

				for (size_t i = 0; i < 32; i++)
				{
					if (m_events.m_keys_up[i] == virtual_key)
					{
						m_events.m_keys_up[i] = 0;
						break;
					}
				}
			}
		}

		/*
		* This methods add a key released in the events struct, it also removed the same key from the keys pressed.
		* @param virtual_key The virtual key to add.
		*/
		void AddKeyReleased(int virtual_key)
		{
			bool already_released = false;

			// Check if the key is already in the array
			for (size_t i = 0; i < 32 && already_released == false; i++)
			{
				if (m_events.m_keys_up[i] == virtual_key)
					already_released = true;
			}
			// If not, we add the key and remove from the keys down array
			if (already_released == false)
			{
				for (size_t i = 0; i < 32; i++)
				{
					if (m_events.m_keys_up[i] == 0)
					{
						m_events.m_keys_up[i] = virtual_key;
						break;
					}
				}
				for (size_t i = 0; i < 32; i++)
				{
					if (m_events.m_keys_down[i] == virtual_key)
					{
						m_events.m_keys_down[i] = 0;
						break;
					}
				}
			}
		}

		/*
		* This methods check if the left mouse button is pressed.
		* @return If the left mouse button is pressed.
		*/
		inline bool IsLeftMouseButtonPressed() const noexcept { return m_events.left_mouse_pressed; }

		/*
		* This methods check if the left mouse button is released.
		* @return If the left mouse button is released.
		*/
		inline bool IsLeftMouseButtonReleased() const noexcept { return m_events.left_mouse_released; }

		/*
		* This methods check if the right mouse button is pressed.
		* @return If the right mouse button is pressed.
		*/
		inline bool IsRightMouseButtonPressed() const noexcept { return m_events.right_mouse_pressed; }

		/*
		* This methods check if the right mouse button is released.
		* @return If the right mouse button is released.
		*/
		inline bool IsRightMouseButtonReleased() const noexcept { return m_events.right_mouse_released; }

		/*
		* This methods check if the middle mouse button is pressed.
		* @return If the middle mouse button is pressed.
		*/
		inline bool IsMiddleMouseButtonPressed() const noexcept { return m_events.middle_mouse_pressed; }

		/*
		* This methods check if the middle mouse button is released.
		* @return If the middle mouse button is released.
		*/
		inline bool IsMiddleMouseButtonReleased() const noexcept { return m_events.middle_mouse_released; }

		/*
		* This methods get the x mouse direction.
		* @return The x mouse direction.
		*/
		inline int GetMouseDirectionX() const noexcept { return m_events.mouse_direction_x; }

		/*
		* This methods get the y mouse direction.
		* @return The y mouse direction.
		*/
		inline int GetMouseDirectionY() const noexcept { return m_events.mouse_direction_y; }
		
		/*
		* This methods get the x mouse position.
		* @return The x mouse position.
		*/
		inline int GetMouseX() const noexcept { return m_events.mouse_x; }
		
		/*
		* This methods get the y mouse position.
		* @return The y mouse position.
		*/
		inline int GetMouseY() const noexcept { return m_events.mouse_y; }

		// Windows only methods
		#if defined(_WIN32) || defined(_WIN64)

		/*
		* This methods get the handle of the display.
		* This methods is only compiled on Windows.
		* @return The y mouse position.
		*/
		inline HWND GetHandle() const noexcept { return m_handle; }

		#endif
	};
}
