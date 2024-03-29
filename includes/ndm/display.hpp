#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>

// NDM includes
#include <ndm/monitor.hpp>
#include <ndm/ndm_utils.hpp>
#include <ndm/os_defines.hpp>

namespace ndm
{
	/**
	* This structure represent all the events happening during the execution by a display, getters for all 
	* of them are defined in the display class directly, all implementations must use this structure to report 
	* all the following events :
	* - display resized, minimized, maximized, moved, closed, language changed
	*/
	struct DisplayEvents
	{
		bool resized;
		bool closed;
		bool minimized;
		bool maximized;
		bool moved;
	};

	/*
	* Enumeration that represent the profile used for the creation of an OpenGL context.
	* There are two types of context : core and compability.
	*/
	enum class GLContextProfile
	{
		CORE_PROFILE,
		COMPATIBILITY_PROFILE
	};

	/*
	* Enumeration that represent the display mode.
	*/
	enum class DisplayMode
	{
		FULLSCREEN,
		WINDOWED
	};

	/**
	* This structure describe the paramaters to create of an OpenGL context for the current thread.
	*/
	struct GLContextParams
	{
		GLContextProfile profile;
		int major_version;
		int minor_version;
		int color_bits;  	    
		int alpha_bits; 
		int depth_bits; 
		int stencil_bits; 
		int samples;
		bool debug_mode;
		bool double_buffer;
		bool samples_buffers;
	};

	/**
	* There is no default implementation of this class, some methods need to be defined in an other class for each systems, also, some methods are shared by all the OS 
	* and are already defined in this header. Each implementation can set attributes or methods in this class but can be only compiled with these, also this library is designed to report 
	* error mainly by returning an enum that tell if there is an error or not.
	*/
	class Display
	{
	private:

		// Attributes
		DisplayEvents m_events;
		NativeAttributes m_native_attributes;
		bool m_loaded = false;

	public:

		/**
		* Constructor of this class.
		* This class is a singleton so if a display already exist, an exception is thrown. If not, the global instance is set.
		*/
		Display() : m_loaded(false) 
		{
			memset(&m_events, 0, sizeof(m_events));
			memset(&m_native_attributes, 0, sizeof(m_native_attributes));
		}

		/**
		* No copy constructor 
		*/
		Display(Display &) = delete;
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
		DisplayEvents CatchEvents() noexcept;

		/**
		* This method load the display.
		* This method need to be implemented for each OS.
		* @param title The title of the display.
		* @param width The width of the display on the screen.
		* @param height The height of the display on the screen.
		*/
		ndm::NDMResult Load(const std::string_view title, const int width, const int height, const bool visible);

		/**
		* This method unload the display by releasing all the associated resources.
		* This method need to be implemented for each OS.
		*/
		ndm::NDMResult Unload();

		/**
		* This method set the display on full screen mode or not.
		* This method need to be implemented for each OS.
		* @param fullscreen If the full screen mode is activated or not.
		*/
		ndm::NDMResult SetDisplayMode(ndm::DisplayMode mode, const ndm::Monitor & monitor);

		/**
		* This method swap the back and front buffer of the display if the display used double buffering.
		* This method need to be implemented for each OS.
		* @param fullscreen The time interval between two swap (0 = no vsync, 1 = vsync).
		*/
		ndm::NDMResult SwapFrontAndBack(const int swap_interval) const noexcept;

		/**
		* This method set the display resizable by the user (minimize, maximize...).
		* This method need to be implemented for each OS.
		* @param resizable If the display is resizable.
		*/
		ndm::NDMResult SetResizableByUser(const bool resizable) noexcept;

		/**
		* This method create an OpenGL context for the current thread.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This method need to be implemented for each OS.
		* @param params The structure that contains all the params for the creation of the OpenGL Context.
		*/
		ndm::NDMResult MakeCurrentThreadOpenGLContext(const GLContextParams & params);
		
		/**
		* This method delete the current OpenGL context.
		* This method need to be implemented for each OS.
		*/
		ndm::NDMResult DeleteOpenGLContext() const;

		/**
		* This method set the title of the display.
		* This method need to be implemented for each OS.
		* @param title The new title of the display.
		*/
		ndm::NDMResult SetTitle(const std::string_view title);

		/**
		* This method set the visibility of the display.
		* This method need to be implemented for each OS.
		* @param visible The visibility of the display.
		*/
		ndm::NDMResult SetVisible(const bool visible);

		/**
		* This method set the display x position on the screen.
		* This method need to be implemented for each OS.
		* @param x The x position
		*/
		ndm::NDMResult SetX(const int x);

		/**
		* This method set the display y position on the screen.
		* This method need to be implemented for each OS.
		* @param y The y display
		*/
		ndm::NDMResult SetY(const int y);

		/**
		* This method set the display width.
		* This method need to be implemented for each OS.
		* @param width The width of the display
		*/
		ndm::NDMResult SetWidth(const int width);

		/**
		* This method set the display height.
		* This method need to be implemented for each OS.
		* @param height The height of the display
		*/
		ndm::NDMResult SetHeight(const int height);

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

		/**
		* This method get the OS-specific attributes of the display.
		* @return The OS-specific attributes of the display.
		*/
		NativeAttributes & GetNativeAttributes()
		{
			return m_native_attributes;
		}

		/**
		* This method return the internal events struct.
		* @return DisplayEvents& The events structure
		*/
		DisplayEvents & GetEvents() noexcept 
		{
			return m_events;
		}
	};
}
