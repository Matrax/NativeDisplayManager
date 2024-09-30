#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>
#include <tuple>

// Win32 NDM includes
#include <ndm/os/win32_functions.hpp>
#include <ndm/monitor/monitor_capabilities.hpp>

namespace ndm
{
    class Monitor
    {
    private:

        #if defined(_WIN32) || defined(_WIN64)

        // Win32 native monitor attributes
        unsigned long m_display_index;
        DISPLAY_DEVICEA m_display_device;
        std::vector<DEVMODEA> m_device_modes;

        #endif

        // Private default constructor
        inline Monitor()
        {
            std::memset(&m_display_device, 0, sizeof(DISPLAY_DEVICEA));
        }

    public:

        // Destructor
        inline virtual ~Monitor() = default;

        static std::vector<Monitor> get_all_monitors();

        std::string get_name() const;

        std::vector<ndm::MonitorCapabilities> get_all_capabilities() const;

        std::tuple<unsigned long, unsigned long> get_max_size() const;

        std::tuple<unsigned long, unsigned long> get_min_size() const;

        std::tuple<unsigned long, unsigned long> get_min_position() const;

        unsigned long get_max_refresh_rate() const;

        unsigned long get_min_refresh_rate() const;

        bool is_primary() const;

        #if defined(_WIN32) || defined(_WIN64)
        const DISPLAY_DEVICEA & get_win32_display_device() const;

        const std::vector<DEVMODEA> & get_win32_device_modes() const;

        unsigned long get_win32_display_index() const;
        #endif

    };
}