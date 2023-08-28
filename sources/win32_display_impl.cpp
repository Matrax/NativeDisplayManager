// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NativeDisplayManager includes
#include <ndm/display.hpp>

LRESULT CALLBACK Win32ProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	NativeDisplayManager::Display * current_display = (NativeDisplayManager::Display *) GetWindowLongPtr(handle, GWLP_USERDATA);
	if(current_display == nullptr)
		return DefWindowProc(handle, message, wParam, lParam);
	NativeDisplayManager::DisplayEvents & events = current_display->GetEvents();
	
	switch (message)
	{
		case WM_CLOSE:
			events.closed = true;
			break;
		case WM_MOUSEMOVE:
			events.moused_moved = true;
			events.previous_mouse_x = events.mouse_x;
			events.previous_mouse_y = events.mouse_y;
			events.mouse_x = static_cast<int>(LOWORD(lParam));
			events.mouse_y = static_cast<int>(HIWORD(lParam));
			events.mouse_direction_x = events.mouse_x - events.previous_mouse_x;
			events.mouse_direction_y = events.mouse_y - events.previous_mouse_y;
			break;
		case WM_SIZE:
			if (wParam == SIZE_RESTORED)
				events.resized = true;
			if (wParam == SIZE_MINIMIZED)
				events.minimized = true;
			if (wParam == SIZE_MAXIMIZED)
				events.maximized = true;
			break;
		case WM_MOVE:
			events.moved = true;
			break;
		case WM_INPUTLANGCHANGE:
			events.language_changed = true;
			break;
		case WM_KEYDOWN:
			current_display->AddKeyPressed((int) wParam);
			break;
		case WM_KEYUP:
			current_display->AddKeyReleased((int) wParam);
			break;
		case WM_LBUTTONDOWN:
			events.left_mouse_pressed = true;
			events.left_mouse_released = false;
			break;
		case WM_LBUTTONUP:
			events.left_mouse_pressed = false;
			events.left_mouse_released = true;
			break;
		case WM_RBUTTONDOWN:
			events.right_mouse_pressed = true;
			events.right_mouse_released = false;
			break;
		case WM_RBUTTONUP:
			events.right_mouse_pressed = false;
			events.right_mouse_released = true;
			break;
		case WM_MBUTTONDOWN:
			events.middle_mouse_pressed = true;
			events.middle_mouse_released = false;
			break;
		case WM_MBUTTONUP:
			events.middle_mouse_pressed = false;
			events.middle_mouse_released = true;
			break;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

namespace NativeDisplayManager
{
	void Display::Load(const std::string_view title, const int width, const int height, const bool visible)
	{
		if (m_loaded == true)
			throw std::runtime_error("Can't load a window that is already loaded !");

		// Get HINSTANCE
		m_instance = GetModuleHandle(nullptr);
		if (m_instance == nullptr)
			throw std::runtime_error("Can't retrieve the HINSTANCE of the application !");

		WNDCLASSEX window_class = {};
		SecureZeroMemory(&window_class, sizeof(WNDCLASSEX));

		if(GetClassInfoEx(m_instance, TEXT("NativeDisplayManagerClass"), &window_class) == 0)
		{
			// Register Win32 class
			
			window_class.lpszClassName = TEXT("NativeDisplayManagerClass");
			window_class.cbSize = sizeof(WNDCLASSEX);
			window_class.lpfnWndProc = Win32ProcessEvent;
			window_class.hInstance = m_instance;
			window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
			window_class.style = CS_OWNDC;
			window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
			window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);

			if (RegisterClassEx(&window_class) == 0)
				throw std::runtime_error("Error on register the windows class !");
		}

		// Create the window
		m_handle = CreateWindowEx(0, TEXT("NativeDisplayManagerClass"), TEXT("Application"),
								WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
								width, height,
								nullptr, nullptr, m_instance, nullptr);

		if (m_handle == nullptr)
			throw std::runtime_error("Error on create the window !");

		// Set USERDATA of the window handle to retrieve the display pointer in the window procedure.
		SetWindowLongPtr(m_handle, GWLP_USERDATA, (LONG_PTR) this);

		// Clear structs
		SecureZeroMemory(&m_events, sizeof(DisplayEvents));
		SecureZeroMemory(&m_messages, sizeof(MSG));

		// Get the device context
		m_device_context = GetDC(m_handle);
		if (m_device_context == nullptr)
			throw std::runtime_error("Can't get the device context !");

		// If visible
		SetVisible(visible);

		// Set title
		SetTitle(title);

		m_loaded = true;
	}

	void Display::SetFullScreen(const bool fullscreen)
	{
		if(m_loaded == false)
			throw std::runtime_error("Can't set the display fullscreen if the display is not loaded !");

		if(fullscreen == true)
		{
			HMONITOR monitor = MonitorFromWindow(m_handle, MONITOR_DEFAULTTOPRIMARY);
			if(monitor == nullptr)
				throw std::runtime_error("Can't get any monitor !");

			MONITORINFOEX monitor_info = {};
			SecureZeroMemory(&monitor_info, sizeof(MONITORINFOEX));
			monitor_info.cbSize = sizeof(MONITORINFO);
			if(GetMonitorInfo(monitor, &monitor_info) == FALSE)
				throw std::runtime_error("Can't get the primary monitor informations !");

			SetWindowLongPtr(m_handle, GWL_STYLE, WS_POPUP);
			SetWindowPos(m_handle, HWND_TOP, 0, 0, monitor_info.rcMonitor.right, monitor_info.rcMonitor.bottom, SWP_SHOWWINDOW);
		} else {
			SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(m_handle, HWND_BOTTOM, 100, 100, 900, 600, SWP_SHOWWINDOW);
		}
	}

	bool Display::HasFocus() const
	{
		return GetActiveWindow() == m_handle;
	}

	// Get the events of the window
	DisplayEvents & Display::CatchEvents() noexcept
	{
		// Clear events and messages
		SecureZeroMemory(&m_events, sizeof(DisplayEvents));
		SecureZeroMemory(&m_messages, sizeof(MSG));

		// While there are windows messages, we dipatch them
		while (PeekMessage(&m_messages, m_handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m_messages);
			DispatchMessage(&m_messages);
		}

		return m_events;
	}

	void Display::SwapFrontAndBack(const int swap_interval) const noexcept
	{
		if (m_loaded == true && m_device_context != nullptr)
		{
			// Set the swap interval
			if (wglSwapIntervalEXT != nullptr)
				wglSwapIntervalEXT(swap_interval);

			// Swap the back and front
			SwapBuffers(m_device_context);
		}
	}

	void Display::MakeOldOpenGLContext(const bool double_buffer, const int color_bits, const int depth_bits, const int stencil_bits)
	{
		if(m_loaded == false)
			throw std::runtime_error("Can't create an OpenGL context if the display is not loaded !");

		// Set pixel format
		PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
		SecureZeroMemory(&pixel_format_descriptor, sizeof(PIXELFORMATDESCRIPTOR));
		pixel_format_descriptor.nVersion = 1;
		pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pixel_format_descriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
		pixel_format_descriptor.cColorBits = color_bits;
		pixel_format_descriptor.cDepthBits = depth_bits;
		pixel_format_descriptor.cStencilBits = stencil_bits;

		if (double_buffer == true)
			pixel_format_descriptor.dwFlags = pixel_format_descriptor.dwFlags | PFD_DOUBLEBUFFER;

		int  pixel_format = ChoosePixelFormat(m_device_context, &pixel_format_descriptor);
		if (SetPixelFormat(m_device_context, pixel_format, &pixel_format_descriptor) == FALSE)
			throw std::runtime_error("Can't set the pixel format !");

		// Create the old GL Context
		m_gl_device_context = wglCreateContext(m_device_context);
		if (m_gl_device_context == nullptr)
			throw std::runtime_error("Can't create the old OpenGL context !");

		// Set the OpenGL context active
		if (wglMakeCurrent(m_device_context, m_gl_device_context) == FALSE)
			throw std::runtime_error("Can't make the old OpenGL context !");
	}

	void Display::MakeOpenGLContext(const int major_version, const int minor_version, const bool double_buffer, const int color_bits, 
							  	    const int alpha_bits, const int depth_bits, const int stencil_bits, const bool samples_buffers, const int samples)
	{
		if(m_loaded == false)
			throw std::runtime_error("Can't create an OpenGL context if the display is not loaded !");

		// Fake window
		HDC fake_device_context = nullptr;
		HGLRC fake_gl_device_context = nullptr;
		HWND fake_handle = nullptr;

		// Register fake Win32 class
		WNDCLASSEX fake_window_class = {};
		SecureZeroMemory(&fake_window_class, sizeof(WNDCLASSEX));
		fake_window_class.lpszClassName = TEXT("FakeWindow");
		fake_window_class.lpfnWndProc = Win32ProcessEvent;
		fake_window_class.hInstance = m_instance;
		fake_window_class.cbSize = sizeof(WNDCLASSEX);
		fake_window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
		fake_window_class.style = CS_OWNDC;

		if (RegisterClassEx(&fake_window_class) == 0)
			throw std::runtime_error("Error on register the fake windows class !");

		// Create the fake window
		fake_handle = CreateWindowEx(0, TEXT("FakeWindow"),
									TEXT("FakeWindow"),
									WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
									0, 0,
									1, 1,
									nullptr, nullptr,
									m_instance, nullptr);

		fake_device_context = GetDC(fake_handle);
		if (fake_device_context == nullptr)
			throw std::runtime_error("Can't get the device context !");

		// Make fake OpenGL context
		PIXELFORMATDESCRIPTOR fake_pixel_format_descriptor = {};
		SecureZeroMemory(&fake_pixel_format_descriptor, sizeof(PIXELFORMATDESCRIPTOR));
		fake_pixel_format_descriptor.nVersion = 1;
		fake_pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		fake_pixel_format_descriptor.dwFlags = PFD_SUPPORT_OPENGL;
		fake_pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
		fake_pixel_format_descriptor.cColorBits = 8;

		int  fake_pixel_format = ChoosePixelFormat(fake_device_context, &fake_pixel_format_descriptor);
		if (SetPixelFormat(fake_device_context, fake_pixel_format, &fake_pixel_format_descriptor) == FALSE)
			throw std::runtime_error("Can't set the fake pixel format !");

		// Create the fake GL Context
		fake_gl_device_context = wglCreateContext(fake_device_context);
		if (fake_gl_device_context == nullptr)
			throw std::runtime_error("Can't create the fake OpenGL context !");

		// Set the fake OpenGL context active
		if (wglMakeCurrent(fake_device_context, fake_gl_device_context) == FALSE)
			throw std::runtime_error("Can't make the fake OpenGL context !");

		// Get WGL extensions functions
		if (wglChoosePixelFormatARB == nullptr || wglCreateContextAttribsARB == nullptr || wglSwapIntervalEXT == nullptr)
		{
			wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (*wglChoosePixelFormatARB == nullptr)
				throw std::runtime_error("Can't get the wglChoosePixelFormatARB function !");

			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			if (*wglCreateContextAttribsARB == nullptr)
				throw std::runtime_error("Can't get the wglCreateContextAttribsARB function !");

			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			if (*wglSwapIntervalEXT == nullptr)
				throw std::runtime_error("Can't get the wglSwapIntervalEXT function !");
		}

		// Delete the fake GL context
		if (wglDeleteContext(fake_gl_device_context) == FALSE)
			throw std::runtime_error("Can't delete the fake OpenGL Context !");

		// Remove the current fake OpenGL context
		wglMakeCurrent(nullptr, nullptr);

		// Destroy the fake window
		if (DestroyWindow(fake_handle) == false)
			throw std::runtime_error("Can't destroy the fake window !");

		// Unregister the fake class
		if (UnregisterClass(L"FakeWindow", m_instance) == 0)
			throw std::runtime_error("Error on unregister the fake window !");

		// Check if the functions are here
		if (wglChoosePixelFormatARB == nullptr || wglCreateContextAttribsARB == nullptr || wglSwapIntervalEXT == nullptr)
			throw std::runtime_error("Can't get the WGL extensions functions !");

		// Make good OpenGL context
		const int pixel_format_attributes[] =
		{
			0x2001, true, //WGL_DRAW_TO_WINDOW_ARB 0x2001
			0x2010, true, // WGL_SUPPORT_OPENGL_ARB 0x2010
			0x2011, double_buffer, // WGL_DOUBLE_BUFFER_ARB 0x2011
			0x2003, 0x2027, // WGL_ACCELERATION_ARB 0x2003 WGL_FULL_ACCELERATION_ARB 0x2027
			0x2013, 0x202B, // WGL_PIXEL_TYPE_ARB 0x2013 WGL_TYPE_RGBA_ARB 0x202B
			0x2014, color_bits, // WGL_COLOR_BITS_ARB 0x2014
			0x2022, depth_bits, // WGL_DEPTH_BITS_ARB 0x2022
			0x201B, alpha_bits, // WGL_ALPHA_BITS_ARB 0x201B
			0x2023, stencil_bits, // WGL_STENCIL_BITS_ARB 0x2023
			0x2041, samples_buffers, // WGL_SAMPLE_BUFFERS_ARB 0x2041
			0x2042, samples, // WGL_SAMPLES_ARB 0x2042
			0
		};

		const int context_attributes[] =
		{
			0x2091, major_version, // WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
			0x2092, minor_version, // WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
			0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB 0x9126 WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
			0
		};

		int pixel_format = -1;
		unsigned int number_of_formats = 0;
		if (wglChoosePixelFormatARB(m_device_context, pixel_format_attributes, nullptr, 1, &pixel_format, &number_of_formats) == FALSE)
			throw std::runtime_error("Can't set the pixel format !");

		PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
		SecureZeroMemory(&pixel_format_descriptor, sizeof(PIXELFORMATDESCRIPTOR));
		if (DescribePixelFormat(m_device_context, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor) == 0)
			throw std::runtime_error("Can't describe the pixel format !");

		if (SetPixelFormat(m_device_context, pixel_format, &pixel_format_descriptor) == false)
			throw std::runtime_error("Can't set the pixel format !");

		m_gl_device_context = wglCreateContextAttribsARB(m_device_context, nullptr, context_attributes);
		if (m_gl_device_context == nullptr)
			throw std::runtime_error("Can't create the OpenGL context !");

		if (wglMakeCurrent(m_device_context, m_gl_device_context) == FALSE)
			throw std::runtime_error("Can't make the OpenGL context !");
	}

	void Display::DeleteOpenGLContext() const
	{
		// Delete the GL context
		if (m_gl_device_context == nullptr)
			throw std::runtime_error("There is no OpenGL context active !");

		if (wglDeleteContext(m_gl_device_context) == FALSE)
			throw std::runtime_error("Can't delete the OpenGL Context");

		// Make the current context null
		wglMakeCurrent(nullptr, nullptr);
	}

	void Display::Unload()
	{
		if (m_loaded == false)
			throw std::runtime_error("Can't unload a window that is already unloaded !");

		m_loaded = false;
	}

	void Display::SetTitle(const std::string_view title)
	{
		// Set window title (unicode version or not)
		#ifdef UNICODE
		std::wstring new_title = std::wstring(title.begin(), title.end());
		#else
		std::string new_title = std::string(title);
		#endif

		if (SetWindowText(m_handle, new_title.c_str()) == false)
			throw std::runtime_error("Can't set the title of the window !");
	}

	void Display::SetX(const int x)
	{
		if (SetWindowPos(m_handle, nullptr, x, GetY(), 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
			throw std::runtime_error("Can't move the window !");
	}

	void Display::SetY(const int y)
	{
		if (SetWindowPos(m_handle, nullptr, GetX(), y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
			throw std::runtime_error("Can't move the window !");
	}

	void Display::SetWidth(const int width)
	{
		if (SetWindowPos(m_handle, nullptr, 0, 0, width, GetHeight(), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
			throw std::runtime_error("Can't move the window !");
	}

	void Display::SetHeight(const int height)
	{
		if (SetWindowPos(m_handle, nullptr, 0, 0, GetWidth(), height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
			throw std::runtime_error("Can't move the window !");
	}

	int Display::GetX() const
	{
		if (m_loaded == false)
			throw std::runtime_error("Can't get the y position because the window is unloaded !");

		RECT rect = { 0, 0, 0, 0 };

		if (GetWindowRect(m_handle, &rect) == FALSE)
			throw std::runtime_error("Can't get the width of the window !");

		return rect.left;
	}

	int Display::GetY() const
	{
		if (m_loaded == false)
			throw std::runtime_error("Can't get the x position because the window is unloaded !");

		RECT rect = { 0, 0, 0, 0 };

		if (GetWindowRect(m_handle, &rect) == FALSE)
			throw std::runtime_error("Can't get the height of the window !");

		return rect.bottom;
	}

	int Display::GetWidth() const
	{
		if (m_loaded == false)
			throw std::runtime_error("Can't get the width because the window is unloaded !");

		RECT rect = { 0, 0, 0, 0 };

		if (GetWindowRect(m_handle, &rect) == FALSE)
			throw std::runtime_error("Can't get the width of the window !");

		return rect.right - rect.left;
	}

	int Display::GetHeight() const
	{
		if (m_loaded == false)
			throw std::runtime_error("Can't get the height because the window is unloaded !");

		RECT rect = { 0, 0, 0, 0 };

		if (GetWindowRect(m_handle, &rect) == FALSE)
			throw std::runtime_error("Can't get the height of the window !");

		return rect.bottom - rect.top;
	}

	void Display::SetCursorPosition(const unsigned long x, const unsigned long y) 
	{ 
		if(GetCursor() == nullptr)
			throw std::runtime_error("There is no cursor available !");

		SetCursorPos(GetX() + x, GetY() + y); 
	}

	void Display::SetCursorPositionToCenter()
	{ 
		if(GetCursor() == nullptr)
			throw std::runtime_error("There is no cursor available !");

		SetCursorPos(GetX() + GetWidth() / 2, GetY() - GetHeight() / 2); 
	}

	void Display::SetCursorVisible(const bool visible)
	{ 
		if(GetCursor() == nullptr)
			throw std::runtime_error("There is no cursor available !");

		ShowCursor(visible);
	}

	void Display::Close() const 
	{ 
		if(m_handle == nullptr)
			throw std::runtime_error("There is no display on the screen !");

		if(m_events.closed == false)
			DestroyWindow(m_handle);
	}

    void Display::SetVisible(const bool visible) 
	{ 
		if(visible == true)
		{
			ShowWindow(m_handle, SW_SHOW);
		} else {
			ShowWindow(m_handle, SW_HIDE);
		}
	}
};

#endif