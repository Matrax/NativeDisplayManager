#pragma once

// STD includes
#include <iostream>
#include <string>
#include <string_view>
#include <exception>
#include <vector>

// NDM includes
#include <ndm/os_defines.hpp>

namespace NativeDisplayManager
{	
	/**
	* This structure represent all the events happening during the execution by a display, getters for all 
	* of them are defined in the display class directly, all implementations must use this structure to report 
	* all the following events :
	* - mouse and keyboard inputs 
	* - mouse position and mouse direction
	* - display resized, minimized, maximized, moved
	* - display closed
	*/
	struct DisplayEvents
	{
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
		int keys_pressed[10];
		int keys_released[10];
	};

	/**
	* There is no default implementation of this class, some methods need to be defined in an other class for each systems, also, some methods are shared by all the OS 
	* and are already defined in this header. Each implementation can set attributes or methods in this class but can be only compiled with these, also this library is designed to report 
	* error mainly with exceptions.
	* Here the list of all implementation :
	* - win32_display_impl.cpp -> implements the Win32 (x32 or x64) version. 
	*/
	class Display
	{

	private:

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

	public:

		/**
		* Constructor of this class.
		* This class is a singleton so if a display already exist, an exception is thrown. If not, the global instance is set.
		*/
		Display() : m_loaded(false) {}

		/**
		* No copy constructor 
		*/
		Display(Display &) = delete;

		/**
		* No copy constructor 
		*/
		Display(const Display&) = delete;

		/**
		* Destructor of this class.
		* If the display is loaded, the destructor unload the display, and then release the global instance.
		*/
		~Display()
		{
			if (m_loaded == true)
				Unload();
		}

		/**
		* This method must return all the events catched by the window.
		* This method need to be implemented for each OS.
		* @return DisplayEvents& The events structure
		*/
		DisplayEvents & CatchEvents() noexcept;

		/**
		* This method return the internal events struct.
		* @return DisplayEvents& The events structure
		*/
		DisplayEvents & GetEvents() noexcept 
		{
			return m_events;
		}

		void ClearEvents() noexcept
		{
			// Mouse events
			m_events.moused_moved = false;
			m_events.left_mouse_pressed = false;
			m_events.left_mouse_released = false;
			m_events.middle_mouse_pressed = false;
			m_events.middle_mouse_released = false;
			m_events.right_mouse_pressed = false;
			m_events.right_mouse_released = false;

			// Window events
			m_events.closed = false;
			m_events.maximized = false;
			m_events.minimized = false;
			m_events.moved = false;
			m_events.resized = false;

			// System events
			m_events.language_changed = false;

			// Keyboards events
			for(size_t i = 0; i < MAX_KEYBOARD_INPUTS; i++)
			{
				m_events.keys_released[i] = 0;
			}
		}

		/**
		* This method load the display.
		* This method need to be implemented for each OS.
		* @param title The title of the display.
		* @param width The width of the display on the screen.
		* @param height The height of the display on the screen.
		*/
		void Load(const std::string_view title, const int width, const int height, const bool visible = false);

		/**
		* This method unload the display by releasing all the associated resources.
		* This method need to be implemented for each OS.
		*/
		void Unload();

		/**
		* This method set the display on full screen mode or not.
		* This method need to be implemented for each OS.
		* @param fullscreen If the full screen mode is activated or not.
		*/
		void SetFullScreen(const bool fullscreen);

		/**
		* This method swap the back and front buffer of the display if the display used double buffering.
		* This method need to be implemented for each OS.
		* @param fullscreen The time interval between two swap (0 = no vsync, 1 = vsync).
		*/
		void SwapFrontAndBack(const int swap_interval) const noexcept;

		/**
		* This method create an OpenGL context in an old way.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This method need to be implemented for each OS.
		* @param double_buffer Use double buffering.
		* @param color_bits Number of bits to represent a color on the color buffer.
		* @param depth_bits Number of bits to represent a data on the depth buffer.
		* @param depth_bits Number of bits to represent a data on the stencil buffer.
		*/
		void MakeOldOpenGLContext(const bool double_buffer, const int color_bits, const int depth_bits, const int stencil_bits);

		/**
		* This method create an OpenGL context.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This method need to be implemented for each OS.
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
							   const int alpha_bits, const int depth_bits, const int stencil_bits, const bool samples_buffers, 
							   const int samples);
		
		/**
		* This method delete the current OpenGL context.
		* This method need to be implemented for each OS.
		*/
		void DeleteOpenGLContext() const;

		/**
		* This method set the title of the display.
		* This method need to be implemented for each OS.
		* @param title The new title of the display.
		*/
		void SetTitle(const std::string_view title);

		/**
		* This method set the cursor position on the display.
		* This method need to be implemented for each OS.
		* @param x The x position of the cursor on the screen.
		* @param y The y position of the cursor on the screen.
		*/
		void SetCursorPosition(const unsigned long x, const unsigned long y);

		/**
		* This method set the cursor position on the center of the display.
		* This method need to be implemented for each OS.
		*/
		void SetCursorPositionToCenter();

		/**
		* This method set the visibility of the cursor on the display.
		* This method need to be implemented for each OS.
		* @param visible The visibility of the cursor.
		*/
		void SetCursorVisible(const bool visible);

		/**
		* This method set the visibility of the display.
		* This method need to be implemented for each OS.
		* @param visible The visibility of the display.
		*/
		void SetVisible(const bool visible);

		/**
		* This method destroy the display on the screen.
		* This method need to be implemented for each OS.
		*/
		void Close() const;

		/**
		* This method set the display x position on the screen.
		* This method need to be implemented for each OS.
		* @param x The x position
		*/
		void SetX(const int x);

		/**
		* This method set the display y position on the screen.
		* This method need to be implemented for each OS.
		* @param y The y display
		*/
		void SetY(const int y);

		/**
		* This method set the display width.
		* This method need to be implemented for each OS.
		* @param width The width of the display
		*/
		void SetWidth(const int width);

		/**
		* This method set the display height.
		* This method need to be implemented for each OS.
		* @param height The height of the display
		*/
		void SetHeight(const int height);

		/**
		* This method check if the display has the focus (if the display is on the top).
		* This method need to be implemented for each OS.
		* @return If the window has the focus.
		*/
		bool HasFocus() const;

		/**
		* This method get the x position of the display on the screen.
		* This method need to be implemented for each OS.
		* @return The x position of the display on the screen.
		*/
		int GetX() const;

		/**
		* This method get the y position of the display on the screen.
		* This method need to be implemented for each OS.
		* @return The y position of the display on the screen.
		*/
		int GetY() const;

		/**
		* This method get the width of the display.
		* This method need to be implemented for each OS.
		* @return The width position of the display.
		*/
		int GetWidth() const;

		/**
		* This method get the height of the display.
		* This method need to be implemented for each OS.
		* @return The height position of the display.
		*/
		int GetHeight() const;

		// Windows only methods
		#if defined(_WIN32) || defined(_WIN64)

			/**
			* This method get the handle of the display.
			* This method is only compiled on Windows.
			* @return The handle of the display.
			*/
			inline HWND GetHandle() const noexcept { return m_handle; }

			/**
			* This method get the device context of the display.
			* This method is only compiled on Windows.
			* @return The device context of the display.
			*/
			inline HDC GetHDC() const noexcept { return m_device_context; }

			/**
			* This method get the OpenGL device context of the display.
			* This method is only compiled on Windows.
			* @return The OpenGL device context of the display.
			*/
			inline HGLRC GetHGLRC() const noexcept { return m_gl_device_context; }

			/**
			* This method get the instance of the program.
			* This method is only compiled on Windows.
			* @return The instance of the program.
			*/
			inline HINSTANCE GetHINSTANCE() const noexcept { return m_instance; }

		#endif
	};
}