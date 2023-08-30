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
    * This structure represent all the informations about a monitor.
    * There are the name, the width, the height of the monitor.
    * It also retrieve if the monitor is the primary monitor.
    */
    struct MonitorInfo
    {
        std::string name;
        unsigned int width;
        unsigned int height;
        bool primary;
    };

    /**
    * This static function get informations about all the monitors.
    * This static function need to be implemented for each OS.
    * @return All informations about all the monitors.
    */
    void RetrieveMonitors(std::vector<MonitorInfo> & monitors); 
}