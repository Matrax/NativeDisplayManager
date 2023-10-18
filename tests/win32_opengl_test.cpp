// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

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
			std::cout << "\tprimary: " << monitors[i].primary << std::endl;
		}

		// Load displays
		display.Load("Window with OpenGL", 900, 600, true);
		display_2.Load("Classic Window", 900, 600, true);

		// Make the current thread an OpenGL context
		NativeDisplayManager::GLContextParams params;
		params.profile = NativeDisplayManager::GLContextProfile::CORE_PROFILE;
		params.debug_mode = false;
		params.major_version = 4;
		params.minor_version = 6;
		params.double_buffer = true;
		params.color_bits = 32;
		params.alpha_bits = 8;
		params.depth_bits = 24; 
		params.stencil_bits = 8;
		params.samples_buffers = false;
		params.samples = 0;
		display.MakeCurrentThreadOpenGLContext(params);

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
		const NativeDisplayManager::DisplayEvents & events = display.CatchEvents();
		const NativeDisplayManager::DisplayEvents & events_2 = display_2.CatchEvents();

		// Check some events for window 1
		if (events.resized == true) 
			std::cout << "display: resized" << std::endl;
		if (events.minimized == true) 
			std::cout << "display: minimized" << std::endl;
		if (events.maximized == true) 
			std::cout << "display: maximized" << std::endl;
		if (events.moved == true) 
			std::cout << "display: moved" << std::endl;
		if (events.language_changed == true) 
			std::cout << "display: language changed" << std::endl;

		// Check some events for window 2
		if (events_2.resized == true) 
			std::cout << "display 2: resized" << std::endl;
		if (events_2.minimized == true) 
			std::cout << "display 2: minimized" << std::endl;
		if (events_2.maximized == true) 
			std::cout << "display 2: maximized" << std::endl;
		if (events_2.moved == true) 
			std::cout << "display 2: moved" << std::endl;
		if (events_2.language_changed == true) 
			std::cout << "display 2: language changed" << std::endl;

		// Update window
		glClear(GL_COLOR_BUFFER_BIT);
		display.SwapFrontAndBack(1);

		// Check window closed
		if (events.closed == true || events_2.closed == true)
		{
			running = false;
			std::cout << "closed" << std::endl;
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

#endif