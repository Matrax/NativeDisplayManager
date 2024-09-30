#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>
#include <tuple>

// Win32 NDM includes
#include <ndm/opengl/gl_context_profile.hpp>
#include <ndm/opengl/gl_context_params.hpp>
#include <ndm/display/display.hpp>
#include <ndm/os/win32_functions.hpp>

namespace ndm
{
    class GLContext
    {
    private:

		// Attributes
        ndm::Display * m_display_ptr;
		bool m_loaded;

		#if defined(_WIN32) || defined(_WIN64)
		// Win32 native display attributes
		HGLRC m_gl_device_context;
		#endif

    public:

        // No default constructor
        inline GLContext() = delete;

        // Constructor
        inline GLContext(ndm::Display * display_ptr) :
            m_display_ptr(display_ptr)
        {
			if(m_display_ptr == nullptr)
				throw std::exception("Can't instantiate a GLContext with a null display !");
		}

        // Destructor
        inline virtual ~GLContext() = default;
        
		/**
		* This method load and create an OpenGL context for the current thread.
		* If the creation of the OpenGL context cannot be done, an exception is thrown.
		* This method need to be implemented for each OS.
		* @param params The structure that contains all the params for the creation of the OpenGL Context.
		*/
		void load(const ndm::GLContextParams & params);
		
		/**
		* This method unload and delete the current OpenGL context.
		* This method need to be implemented for each OS.
		*/
		void unload();

		/**
		* This method swap the back and front buffer of the display if the display use double buffering.
		* This method need to be implemented for each OS.
		*/
		void swap_front_and_back() const;

		void set_vertical_sync(const bool vertical_sync) const;
    };
}