#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>
#include <exception>
#include <ctype.h>

// NDM includes
#include <ndm/display/display_events.hpp>
#include <ndm/display/display_mode.hpp>
#include <ndm/os/win32_functions.hpp>
#include <ndm/monitor/monitor.hpp>


namespace ndm
{
	/**
	* There is no default implementation of this class, some methods need to be defined in an other class for each systems, also, some methods are shared by all the OS 
	* and are already defined in this header. Each implementation can set attributes or methods in this class but can be only compiled with these, also this library is designed to report 
	* error mainly by returning an enum that tell if there is an error or not.
	*/
	class Display
	{
	private:

		// Attributes
		ndm::DisplayEvents m_events;
		bool m_loaded;

		#if defined(_WIN32) || defined(_WIN64)
		// Win32 native display attributes
		HWND m_handle;
		MSG m_messages;
		HDC m_device_context;
		HINSTANCE m_instance;
		#endif

		#if defined(__linux__)
		// X11 native display attributes
		Display * m_display;
		Window m_window;
		XEvent m_events;
		int m_screen;
		unsigned long m_black_pixel;
		unsigned long m_white_pixel;
		#endif

	public:

		/**
		* Constructor of this class.
		* This class is a singleton so if a display already exist, an exception is thrown. If not, the global instance is set.
		*/
		inline Display() : 
			m_loaded(false) 
		{
			std::memset(&m_events, 0, sizeof(ndm::DisplayEvents));
		}

		/**
		* No copy constructors
		*/
		inline Display(Display &) = delete;
		inline Display(const Display&) = delete;

		/**
		* Destructor of this class.
		* If the display is loaded, the destructor unload the display, and then release the global instance.
		*/
		inline ~Display()
		{
			if (m_loaded == true)
				unload();
		}

		/**
		* This method must return all the events catched by the window.
		* This method need to be implemented for each OS.
		* @return DisplayEvents& The events structure
		*/
		ndm::DisplayEvents catch_events() noexcept;

		/**
		* This method load the display.
		* This method need to be implemented for each OS.
		* @param title The title of the display.
		* @param width The width of the display on the screen.
		* @param height The height of the display on the screen.
		*/
		void load(const std::string_view title, const std::uint64_t width, const std::uint64_t height, const bool visible);

		/**
		* This method unload the display by releasing all the associated resources.
		* This method need to be implemented for each OS.
		*/
		void unload();

		/**
		* This method set the display on full screen mode or not.
		* This method need to be implemented for each OS.
		* @param fullscreen If the full screen mode is activated or not.
		*/
		void set_display_mode(ndm::DisplayMode mode, const ndm::Monitor & monitor);

		/**
		* This method set the display resizable by the user (minimize, maximize...).
		* This method need to be implemented for each OS.
		* @param resizable If the display is resizable.
		*/
		void set_resizable_by_user(const bool resizable);

		/**
		* This method set the title of the display.
		* This method need to be implemented for each OS.
		* @param title The new title of the display.
		*/
		void set_title(const std::string_view title);

		/**
		* This method set the visibility of the display.
		* This method need to be implemented for each OS.
		* @param visible The visibility of the display.
		*/
		void set_visible(const bool visible);

		/**
		* This method set the display x position on the screen.
		* This method need to be implemented for each OS.
		* @param x The x position
		*/
		void set_x(const std::uint64_t x);

		/**
		* This method set the display y position on the screen.
		* This method need to be implemented for each OS.
		* @param y The y display
		*/
		void set_y(const std::uint64_t y);

		/**
		* This method set the display width.
		* This method need to be implemented for each OS.
		* @param width The width of the display
		*/
		void set_width(const std::uint64_t width);

		/**
		* This method set the display height.
		* This method need to be implemented for each OS.
		* @param height The height of the display
		*/
		void set_height(const std::uint64_t height);

		/**
		* This method check if the display has the focus (if the display is on the top).
		* This method need to be implemented for each OS.
		* @return If the window has the focus.
		*/
		bool has_focus() const;

		/**
		* This method get the x position of the display on the screen.
		* This method need to be implemented for each OS.
		* @return The x position of the display on the screen.
		*/
		std::int64_t get_x() const;

		/**
		* This method get the y position of the display on the screen.
		* This method need to be implemented for each OS.
		* @return The y position of the display on the screen.
		*/
		std::int64_t get_y() const;

		/**
		* This method get the width of the display.
		* This method need to be implemented for each OS.
		* @return The width position of the display.
		*/
		std::int64_t get_width() const;

		/**
		* This method get the height of the display.
		* This method need to be implemented for each OS.
		* @return The height position of the display.
		*/
		std::int64_t get_height() const;

		/**
		* This method return the internal events struct.
		* @return DisplayEvents & The events structure
		*/
		inline ndm::DisplayEvents & get_events()
		{
			return m_events;
		}

		/**
		* This method return true if the display is loaded.
		* @return bool If the display is loaded or not
		*/
		inline bool is_loaded() const noexcept 
		{
			return m_loaded;
		}

		#if defined(_WIN32) || defined(_WIN64)

		HWND & get_win32_handle();

		HDC & get_win32_device_context();

		HINSTANCE & get_win32_instance();

		#endif
	};
}
