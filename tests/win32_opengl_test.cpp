// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// ndm includes
#include <ndm/display.hpp>
#include <ndm/monitor.hpp>

// GL includes
#include <gl/GL.h>

// STD includes
#include <iostream>

// Main
int main()
{
	// Instantiate window
	ndm::Display display;

	// Load displays
	if(display.Load("Window with OpenGL", 900, 600, true) == ndm::NDMResult::NDM_DISPLAY_ERROR)
		std::cerr << "Can't load the first window !" << std::endl;

	// Make the current thread an OpenGL context for the first window
	ndm::GLContextParams params;
	params.profile = ndm::GLContextProfile::CORE_PROFILE;
	params.debug_mode = false;
	params.major_version = 4;
	params.minor_version = 6;
	params.double_buffer = true;
	params.color_bits = 24;
	params.alpha_bits = 8;
	params.depth_bits = 24; 
	params.stencil_bits = 8;
	params.samples_buffers = false;
	params.samples = 0;
	ndm::NDMResult result = display.MakeCurrentThreadOpenGLContext(params);
	if(result == ndm::NDMResult::NDM_DISPLAY_ERROR || result == ndm::NDMResult::NDM_OPENGL_ERROR)
		std::cerr << "Can't create an OpenGL context !" << std::endl;

	// Set OpenGL clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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

		// Update window
		glClear(GL_COLOR_BUFFER_BIT);
		display.SwapFrontAndBack(1);
	}

	// Unload
	display.DeleteOpenGLContext();
	display.Unload();

	return EXIT_SUCCESS;
}

#endif