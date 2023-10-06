// Only compile on Linux
#if defined(_WIN32) || defined(_WIN64)

#include <ndm/display.hpp>

int main()
{
    NativeDisplayManager::Display display;
    display.Load("test", 1080, 720, true);

    bool running = true;
    while(running == true)
    {
        NativeDisplayManager::DisplayEvents & events = display.CatchEvents();
    }

    display.Unload();

    return EXIT_SUCCESS;
}