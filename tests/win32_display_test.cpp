// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// ndm includes
#include <ndm/display.hpp>
#include <ndm/monitor.hpp>

// STD includes
#include <iostream>

// Main
int main()
{
	// Instantiate window
	ndm::Display display;

	// Load displays
	if(display.Load("Classic window", 900, 600, true) == ndm::NDMResult::NDM_DISPLAY_ERROR)
		std::cerr << "Can't load the first window !" << std::endl;

	// Run
	bool running = true;
	while (running == true)
	{
		// Get Events
		const ndm::DisplayEvents events = display.CatchEvents();

		// Check some events for window 1
		if (events.resized == true) 
			std::cout << "display : resized" << std::endl;
		if (events.minimized == true) 
			std::cout << "display : minimized" << std::endl;
		if (events.maximized == true) 
			std::cout << "display : maximized" << std::endl;
		if (events.moved == true) 
			std::cout << "display : moved" << std::endl;

		// Check window closed
		if (events.closed == true)
		{
			running = false;
			std::cout << "display : closed" << std::endl;
		}
	}

	// Unload
	display.Unload();

	return EXIT_SUCCESS;
}

#endif