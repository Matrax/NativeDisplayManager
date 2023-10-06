// Only compile on Linux
#if defined(__linux__)

#include <ndm/display.hpp>

namespace NativeDisplayManager
{
    void Display::Load(const std::string_view title, const int width, const int height, const bool visible = false)
    {
        // Open the X display
        m_native_attributes.display = XOpenDisplay(nullptr);

        // Get the default screen
        m_native_attributes.screen = DefaultScreen(m_native_attributes.display);

        // Get the black and white pixel compatible with the default screen
        m_native_attributes.black_pixel = BlackPixel(m_native_attributes.display, m_native_attributes.screen);
        m_native_attributes.white_pixel = WhitePixel(m_native_attributes.display, m_native_attributes.screen);
        
        // Create the window
        m_native_attributes.window = XCreateSimpleWindow(m_native_attributes.display, 
                                                        DefaultRootWindow(m_native_attributes.display),
                                                        0, 0,
                                                        width, height, 1,
                                                        m_native_attributes.black_pixel, 
                                                        m_native_attributes.white_pixel);

        // Set the window visible or not                                           
        if(visible == true)
            XMapWindow(m_native_attributes.display, m_native_attributes.window);

        m_loaded = true;
    }

    DisplayEvents & Display::CatchEvents() noexcept
    {
        // Clear events
		ClearEvents();

        // Get all the events
        XNextEvent(m_native_attributes.display, &m_native_attributes.events);

        return m_events;
    }

    void Display::Unload()
    {
		if (m_loaded == false)
			throw std::runtime_error("Can't unload a window that is already unloaded !");

        // Destroy the window
        XDestroyWindow(m_native_attributes.display, m_native_attributes.window);

        // Close the X display
        XCloseDisplay(m_native_attributes.display);

        m_loaded = false;
    }
};

#endif