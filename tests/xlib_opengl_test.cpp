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
}