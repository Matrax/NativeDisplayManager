// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// ndm includes
#include <ndm/monitor.hpp>

BOOL CALLBACK Win32MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{	
    // Retrieve vector pointer
    std::vector<ndm::Monitor> * monitors = (std::vector<ndm::Monitor> *) pData;

    // Get monitor info
    MONITORINFOEX monitor_info = {};
    monitor_info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMon, &monitor_info);

    // Get device name
    const char * device_name = (const char * ) monitor_info.szDevice;

    // Put in the vector
    unsigned int x = lprcMonitor->left;
    unsigned int y = lprcMonitor->top;
    unsigned int width = lprcMonitor->right - lprcMonitor->left;
    unsigned int height = lprcMonitor->bottom - lprcMonitor->top;
    bool is_primary = (monitor_info.dwFlags == MONITORINFOF_PRIMARY);
    monitors->emplace_back(device_name, x, y, width, height, is_primary);

    return TRUE;
}

ndm::Monitor::Monitor(std::string_view name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool is_primary) :
    m_name(name),
    m_x(x),
    m_y(y),
    m_width(width),
    m_height(height),
    m_is_primary(is_primary)
{}

ndm::NDMResult ndm::Monitor::RetrieveMonitors(std::vector<Monitor> & monitors)
{
    monitors.clear();

    EnumDisplayMonitors(NULL, NULL, Win32MonitorProcess, (LPARAM) &monitors);

    return ndm::NDMResult::NDM_OK;
}

#endif