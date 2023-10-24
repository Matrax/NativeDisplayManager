#pragma once

// STD includes
#include <string>
#include <string_view>
#include <vector>

// NDM includes
#include <ndm/ndm_utils.hpp>
#include <ndm/os_defines.hpp>

namespace ndm
{
    /**
    * This class represent all the informations about a monitor.
    * There are the name, the width, the height of the monitor.
    * It also retrieve if the monitor is the primary monitor.
    */
    class Monitor
    {
        private:

            std::string m_name;
            unsigned int m_x;
            unsigned int m_y;
            unsigned int m_width;
            unsigned int m_height;
            bool m_is_primary;

        public:

            Monitor(std::string_view name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool is_primary);

            /**
            * This static function get informations about all the monitors.
            * This static function need to be implemented for each OS.
            * @return All informations about all the monitors.
            */
            static ndm::NDMResult RetrieveMonitors(std::vector<Monitor> & monitors); 

            std::string GetName() const { return m_name; }

            unsigned int GetX() const { return m_x; }

            unsigned int GetY() const { return m_y; }

            unsigned int GetWidth() const { return m_width; }

            unsigned int GetHeight() const { return m_height; }

            bool IsPrimary() const { return m_is_primary; }
    };
}