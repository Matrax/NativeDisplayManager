// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// ndm includes
#include <ndm/monitor.hpp>

// STD includes
#include <iostream>

// Main
int main()
{
	// Retrieve monitors
	std::vector<ndm::Monitor> monitors;
	if(ndm::Monitor::RetrieveMonitors(monitors) == ndm::NDMResult::NDM_MONITOR_ERROR)
		std::cerr << "Can't retrieve all the monitors !" << std::endl;

	// Show info of all monitors
	std::cout << monitors.size() << " monitor(s) founded" << std::endl;
	for(size_t i = 0; i < monitors.size(); i++)
	{
		std::cout << "==================" << std::endl;
		std::cout << "Monitor " << i << std::endl;
		std::cout << "name: " << monitors[i].GetName() << std::endl;
		std::cout << "width: " << monitors[i].GetWidth() << std::endl;
		std::cout << "height: " << monitors[i].GetHeight() << std::endl;
		std::cout << "primary: " << monitors[i].IsPrimary() << std::endl;
	}
	std::cout << "==================" << std::endl;

	return EXIT_SUCCESS;
}

#endif