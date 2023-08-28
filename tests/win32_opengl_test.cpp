// NativeDisplayManager includes
#include <ndm/display.hpp>
#include <ndm/monitor.hpp>

// GL includes
#include <gl/GL.h>

// Main
int main()
{
	// Window
	NativeDisplayManager::Display display;
	NativeDisplayManager::Display display_2;

	//Load
	try {
		
		// Retrieve monitors
		std::vector<NativeDisplayManager::MonitorInfo> monitors;
		NativeDisplayManager::RetrieveMonitors(monitors);

		for(size_t i = 0; i < monitors.size(); i++)
		{
			std::cout << "Monitor " << i << std::endl;
			std::cout << "\tname: " << monitors[i].name << std::endl;
			std::cout << "\twidth: " << monitors[i].width << std::endl;
			std::cout << "\theight: " << monitors[i].height << std::endl;
		}

		// Load displays
		display_2.Load("Test", 900, 600, true);
		display.Load("Test", 900, 600, true);
		display.MakeOpenGLContext(4, 6, true, 32, 8, 24, 8, false, 0);
		display.SetTitle((const char *) glGetString(GL_VERSION));

		// Set OpenGL clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	} catch (const std::exception & exception) {
		std::cout << exception.what() << std::endl;
	}

	// Run
	bool running = true;
	while (running == true)
	{
		// Get Events
		NativeDisplayManager::DisplayEvents & events = display.CatchEvents();
		NativeDisplayManager::DisplayEvents & events_2 = display_2.CatchEvents();

		// Check some events for window 1
		if (events.resized == true) 
			std::cout << "event: resized" << std::endl;
		if (events.minimized == true) 
			std::cout << "event: minimized" << std::endl;
		if (events.maximized == true) 
			std::cout << "event: maximized" << std::endl;
		if (events.moved == true) 
			std::cout << "event: moved" << std::endl;
		if (events.language_changed == true) 
			std::cout << "event: language changed" << std::endl;

		// Check some events for window 2
		if (events_2.resized == true) 
			std::cout << "event 2: resized" << std::endl;
		if (events_2.minimized == true) 
			std::cout << "event 2: minimized" << std::endl;
		if (events_2.maximized == true) 
			std::cout << "event 2: maximized" << std::endl;
		if (events_2.moved == true) 
			std::cout << "event 2: moved" << std::endl;
		if (events_2.language_changed == true) 
			std::cout << "event 2: language changed" << std::endl;

		// Update window
		glClear(GL_COLOR_BUFFER_BIT);
		display.SwapFrontAndBack(1);

		// Check window closed
		if (events.closed == true)
		{
			running = false;
			std::cout << "event: closed" << std::endl;
		}

		// Check window 2 closed
		if (events_2.closed == true)
		{
			running = false;
			std::cout << "event 2: closed" << std::endl;
		}
	}

	// Unload
	try {
		display.DeleteOpenGLContext();
		display.Unload();
		display_2.Unload();
	} catch (const std::exception& exception) {
		std::cout << exception.what() << std::endl;
	}

	return EXIT_SUCCESS;
}