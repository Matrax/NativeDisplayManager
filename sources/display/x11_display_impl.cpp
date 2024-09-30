// Only compile on Linux
#if defined(__linux__)

#include <ndm/m_display.hpp>

namespace ndm
{
    void m_display::Load(const std::string_view title, const int width, const int height, const bool visible = false)
    {
        // Open the X m_display
        m_display = XOpenDisplay(nullptr);

        // Get the default m_screen
        m_screen = DefaultScreen(m_display);

        // Get the black and white pixel compatible with the default m_screen
        m_black_pixel = BlackPixel(m_display, m_screen);
        m_white_pixel = WhitePixel(m_display, m_screen);
        
        // Create the window
        m_window = XCreateSimpleWindow(m_display, 
                                        DefaultRootWindow(m_display),
                                        0, 0,
                                        width, height, 1,
                                        m_black_pixel, 
                                        m_white_pixel);

        // Set the m_window visible or not                                           
        if(visible == true)
            XMapWindow(m_display, m_window);

        m_loaded = true;
    }

    DisplayEvents & m_display::CatchEvents() noexcept
    {
        // Get all the events
        XNextEvent(m_display, &m_events);

        return m_events;
    }

    void m_display::Unload()
    {
		if (m_loaded == false)
			throw std::runtime_error("Can't unload a m_window that is already unloaded !");

        // Destroy the m_window
        XDestroyWindow(m_display, m_window);

        // Close the X m_display
        XCloseDisplay(m_display);

        m_loaded = false;
    }
};

#endif