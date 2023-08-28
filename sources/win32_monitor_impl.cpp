// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NativeDisplayManager includes
#include <ndm/monitor.hpp>

BOOL CALLBACK Win32MonitorProcess(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{	
    // Retrieve vector pointer
    std::vector<NativeDisplayManager::MonitorInfo> * monitors = (std::vector<NativeDisplayManager::MonitorInfo> *) pData;

    // Get monitor info
    MONITORINFOEX monitor_info = {};
    monitor_info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMon, &monitor_info);

    // Fil the result structure
    NativeDisplayManager::MonitorInfo info = {};
    info.name = std::string((char *) monitor_info.szDevice);
    info.width = lprcMonitor->right - lprcMonitor->left;
    info.height = lprcMonitor->bottom - lprcMonitor->top;
    monitors->push_back(info);

    return TRUE;
}

void NativeDisplayManager::RetrieveMonitors(std::vector<MonitorInfo> & monitors)
{
    monitors.clear();

    if(EnumDisplayMonitors(NULL, NULL, Win32MonitorProcess, (LPARAM) &monitors) == 0)
        throw std::runtime_error("Can't retrieve all the monitors !");

    monitors.shrink_to_fit();
}

#endif