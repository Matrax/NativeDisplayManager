// NativeDisplayManager includes
#include <display.hpp>

// GL includes
#include <gl/GL.h>

// Main
int main()
{
	// Window
	NativeDisplayManager::Display display;

	//Load
	try {
		display.Load("Test", 900, 600);
		display.MakeOpenGLContext(4, 6, true, 32 ,8, 24, 8, true, 1);
		display.SetTitle((const char *) glGetString(GL_VERSION));
		display.Show();
	} catch (const std::exception & exception){
		std::cout << exception.what() << std::endl;
	}

	// Run
	bool running = true;
	NativeDisplayManager::DisplayEvents events = {};
	while (running == true)
	{
		// Get Events
		events = display.GetEvents();

		// Check some events
		if (events.resized == true) std::cout << "event: resized" << std::endl;
		if (events.minimized == true) std::cout << "event: minimized" << std::endl;
		if (events.maximized == true) std::cout << "event: maximized" << std::endl;
		if (events.moved == true) std::cout << "event: moved" << std::endl;
		if (events.language_changed == true) std::cout << "event: language changed" << std::endl;

		// Update window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		display.SwapFrontAndBack(1);

		// Check window closed
		if (events.closed == true)
		{
			running = false;
			std::cout << "event: closed" << std::endl;
		}
	}

	// Unload
	try {
		display.Hide();
		display.DeleteOpenGLContext();
		display.Unload();
	} catch (const std::exception& exception) {
		std::cout << exception.what() << std::endl;
	}

	return EXIT_SUCCESS;
}