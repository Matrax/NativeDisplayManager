// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NDM includes
#include <ndm/display/display.hpp>
#include <ndm/opengl/gl_context.hpp>
#include <ndm/monitor/monitor.hpp>

// STD includes
#include <iostream>

// GL includes
#include <gl/GL.h>

// Main
int main()
{
	try {
		// Create the display
		ndm::Display display;
		display.load("Classic window", 900, 600, true);

		// GL Context
		ndm::GLContext gl_context(&display);
		ndm::GLContextParams params = {};
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
		gl_context.load(params);
		gl_context.set_vertical_sync(true);

		std::cout << glGetString(GL_VERSION) << std::endl;
		glClearColor(1.0f, 0, 0, 1);

		// Run
		bool running = true;
		while (running == true)
		{
			// Get Events
			const ndm::DisplayEvents events = display.catch_events();

			// Check some events
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

			// Test OpenGL
			glClear(GL_COLOR_BUFFER_BIT);
			gl_context.swap_front_and_back();
		}

		// Unload
		gl_context.unload();
		display.unload();
	} catch(const std::exception & exception) {
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

#endif