// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NDM includes
#include <ndm/monitor/monitor.hpp>

std::vector<ndm::Monitor> ndm::Monitor::get_all_monitors()
{
    // Instantiate the list of monitors
    std::vector<ndm::Monitor> monitors;

    // Enumerate every monitors
    unsigned int monitor_index = 0;
    DISPLAY_DEVICEA display_device = {};
    display_device.cb = sizeof(DISPLAY_DEVICEA);

    // Iterate every monitors available 
    while(EnumDisplayDevicesA(nullptr, monitor_index++, &display_device, 0) > 0)
    {
        // If the device is an active monitor
        if((display_device.StateFlags & DISPLAY_DEVICE_ACTIVE) == DISPLAY_DEVICE_ACTIVE)
        {
            // Fill monitor data
            ndm::Monitor monitor;
            monitor.m_display_index = monitor_index;
            monitor.m_display_device = display_device;

            // Enum every display mode of the monitor
            unsigned int mode_index = 0;
            DEVMODEA device_mode = {};
            device_mode.dmSize = sizeof(DEVMODEA);
            while(EnumDisplaySettingsA(display_device.DeviceName, mode_index++, &device_mode) > 0)
            {
                // Add the capabilities to the list
                monitor.m_device_modes.push_back(device_mode);
            }
            
            // Add the monitor in the list
            monitors.push_back(monitor);
        }
    }

    return monitors;
}

std::string ndm::Monitor::get_name() const
{
    return std::string(m_display_device.DeviceName);
}

std::vector<ndm::MonitorCapabilities> ndm::Monitor::get_all_capabilities() const
{
    std::vector<ndm::MonitorCapabilities> monitor_capabilities;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        ndm::MonitorCapabilities capabilities = {};
        std::memset(&monitor_capabilities, 0, sizeof(ndm::MonitorCapabilities));
        capabilities.m_width = device_mode.dmPelsWidth;
        capabilities.m_height = device_mode.dmPelsHeight;
        capabilities.m_x = device_mode.dmPosition.x;
        capabilities.m_y = device_mode.dmPosition.y;
        capabilities.refresh_rate = device_mode.dmDisplayFrequency;
    }

    return monitor_capabilities;
}

std::tuple<unsigned long, unsigned long> ndm::Monitor::get_max_size() const
{
    unsigned long width = 0;
    unsigned long height = 0;
    unsigned long max_size = 0;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        unsigned long current = device_mode.dmPelsWidth * device_mode.dmPelsHeight;

        if(current > max_size)
        {
            width = device_mode.dmPelsWidth;
            height = device_mode.dmPelsHeight;
            max_size = current;
        }
    }

    return std::tuple<unsigned long, unsigned long>(width, height);
}

std::tuple<unsigned long, unsigned long> ndm::Monitor::get_min_size() const
{
    unsigned long width = 0;
    unsigned long height = 0;
    unsigned long min_size = ULONG_MAX;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        unsigned long current = device_mode.dmPelsWidth * device_mode.dmPelsHeight;

        if(current < min_size)
        {
            width = device_mode.dmPelsWidth;
            height = device_mode.dmPelsHeight;
            min_size = current;
        }
    }

    return std::tuple<unsigned long, unsigned long>(width, height);
}

std::tuple<unsigned long, unsigned long> ndm::Monitor::get_min_position() const
{
    unsigned long x = 0;
    unsigned long y = 0;
    unsigned long min_position = ULONG_MAX;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        unsigned long current = device_mode.dmPosition.x + device_mode.dmPosition.y;

        if(current < min_position)
        {
            x = device_mode.dmPosition.x;
            y = device_mode.dmPosition.y;
            min_position = current;
        }
    }

    return std::tuple<unsigned long, unsigned long>(x, y);
}

unsigned long ndm::Monitor::get_max_refresh_rate() const
{
    unsigned long max_refresh_rate = 0;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        if(device_mode.dmDisplayFrequency > max_refresh_rate)
            max_refresh_rate = device_mode.dmDisplayFrequency;
    }

    return max_refresh_rate;
}

unsigned long ndm::Monitor::get_min_refresh_rate() const
{
    unsigned long max_refresh_rate = ULONG_MAX;

    for(const DEVMODEA & device_mode : m_device_modes)
    {
        if(device_mode.dmDisplayFrequency < max_refresh_rate)
            max_refresh_rate = device_mode.dmDisplayFrequency;
    }

    return max_refresh_rate;
}

bool ndm::Monitor::is_primary() const
{
    return (m_display_device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == DISPLAY_DEVICE_PRIMARY_DEVICE;
}

const DISPLAY_DEVICEA & ndm::Monitor::get_win32_display_device() const
{
    return m_display_device;
}

const std::vector<DEVMODEA> & ndm::Monitor::get_win32_device_modes() const
{
    return m_device_modes;
}

unsigned long ndm::Monitor::get_win32_display_index() const
{
    return m_display_index;
}

#endif