// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NDM includes
#include <ndm/monitor/monitor.hpp>

// STD includes
#include <iostream>

// Main
int main()
{
	// Retrieve all monitors
	std::vector<ndm::Monitor> monitors = ndm::Monitor::get_all_monitors();

	// Show info of all monitors
	std::cout << monitors.size() << " monitor(s) founded" << std::endl;
	for(size_t i = 0; i < monitors.size(); i++)
	{
		std::cout << "==================" << std::endl;
		std::cout << "Monitor " << i << std::endl;
		std::cout << "name: " << monitors[i].get_name() << std::endl;
		std::cout << "min size: " << std::get<0>(monitors[i].get_min_size()) << "x" << std::get<1>(monitors[i].get_min_size()) << std::endl;
		std::cout << "max size: " << std::get<0>(monitors[i].get_max_size()) << "x" << std::get<1>(monitors[i].get_max_size()) << std::endl;
		std::cout << "min refresh rate: " << monitors[i].get_min_refresh_rate() << std::endl;
		std::cout << "max refresh rate: " << monitors[i].get_max_refresh_rate() << std::endl;
		std::cout << "primary: " << monitors[i].is_primary() << std::endl;
	}
	std::cout << "==================" << std::endl;

	return EXIT_SUCCESS;
}

#endif