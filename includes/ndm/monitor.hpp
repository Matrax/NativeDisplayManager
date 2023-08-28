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
    struct MonitorInfo
    {
        std::string name;
        unsigned int width;
        unsigned int height;
    };

    /**
    * This static function get informations about a monitor specified with the id.
    * This static function need to be implemented for each OS.
    * @return All informations about a monitor specified with the id.
    */
    void RetrieveMonitors(std::vector<MonitorInfo> & monitors); 
}