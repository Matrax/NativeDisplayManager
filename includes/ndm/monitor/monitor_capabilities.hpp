#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <tuple>

// Win32 NDM includes
#include <ndm/os/win32_functions.hpp>

namespace ndm
{
    struct MonitorCapabilities
    {
        unsigned long m_width;
        unsigned long m_height;
        unsigned long m_x;
        unsigned long m_y;
        unsigned long refresh_rate;
    };
}