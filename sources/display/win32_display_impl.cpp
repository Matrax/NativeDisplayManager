// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NDM includes
#include <ndm/display/display.hpp>

LRESULT CALLBACK ndm::win32_process_events(HWND m_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	ndm::Display * current_display = (ndm::Display *) GetWindowLongPtr(m_handle, GWLP_USERDATA);
	if(current_display == nullptr)
		return DefWindowProc(m_handle, message, wParam, lParam);
	ndm::DisplayEvents & events = current_display->get_events();
	
	switch (message)
	{
		case WM_CLOSE:
			events.closed = true;
			break;
		case WM_SIZE:
			if (wParam == SIZE_RESTORED) events.resized = true;
			if (wParam == SIZE_MINIMIZED) events.minimized = true;
			if (wParam == SIZE_MAXIMIZED) events.maximized = true;
			break;
		case WM_MOVE:
			events.moved = true;
			break;
	}

	return DefWindowProc(m_handle, message, wParam, lParam);
}

void ndm::Display::load(const std::string_view title, const std::uint64_t width, const std::uint64_t height, const bool visible)
{
	// Get HINSTANCE
	m_instance = GetModuleHandle(nullptr);
	if (m_instance == nullptr)
		throw std::exception("Can't retrieve the HINSTANCE !");

	// Register the NDM window class
	WNDCLASSEXA window_class = {};
	std::memset(&window_class, 0, sizeof(WNDCLASSEXA));
	if(GetClassInfoExA(m_instance, "NDMClass", &window_class) == 0)
	{
		// Register Win32 class
		window_class.lpszClassName = "NDMClass";
		window_class.cbSize = sizeof(WNDCLASSEXA);
		window_class.lpfnWndProc = ndm::win32_process_events;
		window_class.hInstance = m_instance;
		window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
		window_class.style = CS_OWNDC;
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);

		if (RegisterClassExA(&window_class) == 0)
			throw std::exception("Can't register the window class !");
	}

	// Create the window
	m_handle = CreateWindowExA(0, "NDMClass", "Application",
							   WS_OVERLAPPEDWINDOW, 
							   CW_USEDEFAULT, CW_USEDEFAULT,
							   static_cast<int>(width), static_cast<int>(height),
							   nullptr, nullptr, m_instance, nullptr);
	if (m_handle == nullptr)
		throw std::exception("Can't create the window !");

	// Set USERDATA of the window m_handle to retrieve the display pointer in the window procedure.
	SetWindowLongPtr(m_handle, GWLP_USERDATA, (LONG_PTR) this);

	// Clear structs
	std::memset(&m_events, 0, sizeof(DisplayEvents));
	std::memset(&m_messages, 0, sizeof(MSG));

	// Get the device context
	m_device_context = GetDC(m_handle);
	if (m_device_context == nullptr)
		throw std::exception("Can't retrieve the device context !");

	// Set loaded
	m_loaded = true;

	// Set title and visibility
	set_visible(visible);
	set_title(title);
}

void ndm::Display::set_display_mode(ndm::DisplayMode mode, const ndm::Monitor & monitor)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	switch (mode)
	{
	case ndm::DisplayMode::FULLSCREEN:
		SetWindowLongPtr(m_handle, GWL_STYLE, WS_POPUP);
		SetWindowPos(m_handle, HWND_TOP, 
					std::get<0>(monitor.get_min_position()), std::get<1>(monitor.get_min_position()), 
					std::get<0>(monitor.get_max_size()), std::get<1>(monitor.get_max_size()), 
					SWP_SHOWWINDOW);
		break;
	case ndm::DisplayMode::WINDOWED:
		SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_handle, HWND_BOTTOM, 
					std::get<0>(monitor.get_min_position()), std::get<1>(monitor.get_min_position()), 
					std::get<0>(monitor.get_min_size()), std::get<1>(monitor.get_min_size()), 
					SWP_SHOWWINDOW);
		break;
	}
}

bool ndm::Display::has_focus() const
{
	return GetActiveWindow() == m_handle;
}

void ndm::Display::set_resizable_by_user(const bool resizable) 
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if(resizable == true)
	{
		SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	} else {
		SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
	}

	SetWindowPos(m_handle, HWND_TOPMOST, 
				 static_cast<int>(get_x()), static_cast<int>(get_y()), 
				 static_cast<int>(get_width()), static_cast<int>(get_height()), 
				 SWP_SHOWWINDOW);
}

ndm::DisplayEvents ndm::Display::catch_events() noexcept
{
	// Clear all events
	std::memset(&m_events, 0, sizeof(ndm::DisplayEvents));

	// While there are windows m_messages, we dipatch them
	while (PeekMessage(&m_messages, m_handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_messages);
		DispatchMessage(&m_messages);
	}

	return m_events;
}

void ndm::Display::unload()
{
	if (m_loaded == false)
		throw std::exception("The display is not loaded !");

	if(m_handle == nullptr)
		throw std::exception("There is no handle !");

	if(m_events.closed == false)
		DestroyWindow(m_handle);
		
	m_loaded = false;
}

void ndm::Display::set_title(const std::string_view title)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	std::string new_title = std::string(title);
	if (SetWindowTextA(m_handle, new_title.c_str()) == false)
		throw std::exception("Can't set the title of the window !");
}

void ndm::Display::set_visible(const bool visible) 
{ 
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if(visible == true)
	{
		ShowWindow(m_handle, SW_SHOW);
	} else {
		ShowWindow(m_handle, SW_HIDE);
	}
}

void ndm::Display::set_x(const std::uint64_t x)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if (SetWindowPos(m_handle, nullptr, static_cast<int>(x), static_cast<int>(get_y()), 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		throw std::exception("Can't set the X position of the window !");
}

void ndm::Display::set_y(const std::uint64_t y)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if (SetWindowPos(m_handle, nullptr, static_cast<int>(get_x()), static_cast<int>(y), 0, 0, 
					 SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		throw std::exception("Can't set the Y position of the window !");
}

void ndm::Display::set_width(const std::uint64_t width)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if (SetWindowPos(m_handle, nullptr, 0, 0, static_cast<int>(width), static_cast<int>(get_height()), 
					 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		throw std::exception("Can't set the width of the window !");
}

void ndm::Display::set_height(const std::uint64_t height)
{
	if(m_loaded == false)
		throw std::exception("The display is not loaded !");

	if (SetWindowPos(m_handle, nullptr, 0, 0, static_cast<int>(get_width()), static_cast<int>(height), 
					 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		throw std::exception("Can't set the height of the window !");
}

std::int64_t ndm::Display::get_x() const
{
	if (m_loaded == false)
		return -1l;

	RECT rect = { 0, 0, 0, 0 };
	if (GetWindowRect(m_handle, &rect) == FALSE)
		return -1l;

	return rect.left;
}

std::int64_t ndm::Display::get_y() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };
	if (GetWindowRect(m_handle, &rect) == FALSE)
		return -1;

	return rect.bottom;
}

std::int64_t ndm::Display::get_width() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };
	if (GetWindowRect(m_handle, &rect) == FALSE)
		return -1;

	return rect.right - rect.left;
}

std::int64_t ndm::Display::get_height() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };
	if (GetWindowRect(m_handle, &rect) == FALSE)
		return -1;

	return rect.bottom - rect.top;
}

HWND & ndm::Display::get_win32_handle()
{
	return m_handle;
}

HDC & ndm::Display::get_win32_device_context()
{
	return m_device_context;
}

HINSTANCE & ndm::Display::get_win32_instance()
{
	return m_instance;
}

#endif