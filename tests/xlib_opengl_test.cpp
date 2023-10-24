// Only compile on Linux
#if defined(__linux__)

#include <ndm/display.hpp>

int main()
{
    ndm::Display display;
    display.Load("test", 1080, 720, true);

    bool running = true;
    while(running == true)
    {
        ndm::DisplayEvents & events = display.CatchEvents();
    }

    display.Unload();

    return EXIT_SUCCESS;
}

#endif