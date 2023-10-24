// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// ndm includes
#include <ndm/display.hpp>

LRESULT CALLBACK Win32ProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	ndm::Display * current_display = (ndm::Display *) GetWindowLongPtr(handle, GWLP_USERDATA);
	if(current_display == nullptr)
		return DefWindowProc(handle, message, wParam, lParam);
	ndm::DisplayEvents & events = current_display->GetEvents();
	
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

	return DefWindowProc(handle, message, wParam, lParam);
}


ndm::NDMResult ndm::Display::Load(const std::string_view title, const int width, const int height, const bool visible)
{
	// Get HINSTANCE
	m_native_attributes.instance = GetModuleHandle(nullptr);
	if (m_native_attributes.instance == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	WNDCLASSEX window_class = {};
	SecureZeroMemory(&window_class, sizeof(WNDCLASSEX));

	if(GetClassInfoEx(m_native_attributes.instance, TEXT("ndmClass"), &window_class) == 0)
	{
		// Register Win32 class
		
		window_class.lpszClassName = TEXT("ndmClass");
		window_class.cbSize = sizeof(WNDCLASSEX);
		window_class.lpfnWndProc = Win32ProcessEvent;
		window_class.hInstance = m_native_attributes.instance;
		window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
		window_class.style = CS_OWNDC;
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);

		if (RegisterClassEx(&window_class) == 0)
			return ndm::NDMResult::NDM_DISPLAY_ERROR;
	}

	// Create the window
	m_native_attributes.handle = CreateWindowEx(0, TEXT("ndmClass"), TEXT("Application"),
							WS_OVERLAPPEDWINDOW, 
							CW_USEDEFAULT, CW_USEDEFAULT,
							width, height,
							nullptr, nullptr, m_native_attributes.instance, nullptr);

	if (m_native_attributes.handle == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Set USERDATA of the window handle to retrieve the display pointer in the window procedure.
	SetWindowLongPtr(m_native_attributes.handle, GWLP_USERDATA, (LONG_PTR) this);

	// Clear structs
	SecureZeroMemory(&m_events, sizeof(DisplayEvents));
	SecureZeroMemory(&m_native_attributes.messages, sizeof(MSG));

	// Get the device context
	m_native_attributes.device_context = GetDC(m_native_attributes.handle);
	if (m_native_attributes.device_context == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;


	// Set loaded
	m_loaded = true;

	// Set title and visibility
	SetVisible(visible);
	SetTitle(title);

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetDisplayMode(ndm::DisplayMode mode, const ndm::Monitor & monitor)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	switch (mode)
	{
	case ndm::DisplayMode::FULLSCREEN:
		SetWindowLongPtr(m_native_attributes.handle, GWL_STYLE, WS_POPUP);
		SetWindowPos(m_native_attributes.handle, HWND_TOP, monitor.GetX(), monitor.GetY(), monitor.GetWidth(), monitor.GetHeight(), SWP_SHOWWINDOW);
		break;
	case ndm::DisplayMode::WINDOWED:
		SetWindowLongPtr(m_native_attributes.handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_native_attributes.handle, HWND_BOTTOM, 100, 100, 900, 600, SWP_SHOWWINDOW);
		break;
	}

	return ndm::NDMResult::NDM_OK;
}

bool ndm::Display::HasFocus() const
{
	return GetActiveWindow() == m_native_attributes.handle;
}

ndm::NDMResult ndm::Display::SetResizableByUser(const bool resizable) noexcept
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if(resizable == true)
	{
		SetWindowLongPtr(m_native_attributes.handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	} else {
		SetWindowLongPtr(m_native_attributes.handle, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
	}

	SetWindowPos(m_native_attributes.handle, HWND_TOPMOST, GetX(), GetY(), GetWidth(), GetHeight(), SWP_SHOWWINDOW);

	return ndm::NDMResult::NDM_OK;
}

// Get the events of the window
ndm::DisplayEvents ndm::Display::CatchEvents() noexcept
{
	// Clear all events
	SecureZeroMemory(&m_events, sizeof(ndm::DisplayEvents));

	// While there are windows messages, we dipatch them
	while (PeekMessage(&m_native_attributes.messages, m_native_attributes.handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_native_attributes.messages);
		DispatchMessage(&m_native_attributes.messages);
	}

	return m_events;
}

ndm::NDMResult ndm::Display::SwapFrontAndBack(const int swap_interval) const noexcept
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if(m_native_attributes.device_context == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Set the swap interval
	if (wglSwapIntervalEXT != nullptr)
		wglSwapIntervalEXT(swap_interval);

	// Swap the back and front
	SwapBuffers(m_native_attributes.device_context);

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::MakeCurrentThreadOpenGLContext(const GLContextParams & params)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Fake window
	HDC fake_device_context = nullptr;
	HGLRC fake_gl_device_context = nullptr;
	HWND fake_handle = nullptr;

	// Register fake Win32 class
	WNDCLASSEX fake_window_class = {};
	SecureZeroMemory(&fake_window_class, sizeof(WNDCLASSEX));
	fake_window_class.lpszClassName = TEXT("FakeWindow");
	fake_window_class.lpfnWndProc = Win32ProcessEvent;
	fake_window_class.hInstance = m_native_attributes.instance;
	fake_window_class.cbSize = sizeof(WNDCLASSEX);
	fake_window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
	fake_window_class.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	if (RegisterClassEx(&fake_window_class) == 0)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Create the fake window
	fake_handle = CreateWindowEx(0, TEXT("FakeWindow"),
								TEXT("FakeWindow"),
								WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0,
								1, 1,
								nullptr, nullptr,
								m_native_attributes.instance, nullptr);

	fake_device_context = GetDC(fake_handle);
	if (fake_device_context == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

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
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Create the fake GL Context
	fake_gl_device_context = wglCreateContext(fake_device_context);
	if (fake_gl_device_context == nullptr)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	// Set the fake OpenGL context active
	if (wglMakeCurrent(fake_device_context, fake_gl_device_context) == FALSE)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	// Get WGL extensions functions
	if (wglChoosePixelFormatARB == nullptr || wglCreateContextAttribsARB == nullptr || wglSwapIntervalEXT == nullptr)
	{
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if (*wglChoosePixelFormatARB == nullptr)
			return ndm::NDMResult::NDM_OPENGL_ERROR;

		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (*wglCreateContextAttribsARB == nullptr)
			return ndm::NDMResult::NDM_OPENGL_ERROR;

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (*wglSwapIntervalEXT == nullptr)
			return ndm::NDMResult::NDM_OPENGL_ERROR;
	}

	// Delete the fake GL context
	if (wglDeleteContext(fake_gl_device_context) == FALSE)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	// Remove the current fake OpenGL context
	wglMakeCurrent(nullptr, nullptr);

	// Destroy the fake window
	if (DestroyWindow(fake_handle) == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Unregister the fake class
	if (UnregisterClass(L"FakeWindow", m_native_attributes.instance) == 0)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Check if the functions are here
	if (wglChoosePixelFormatARB == nullptr || wglCreateContextAttribsARB == nullptr || wglSwapIntervalEXT == nullptr)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	// Make good OpenGL context
	const int pixel_format_attributes[] =
	{
		0x2001, true, //WGL_DRAW_TO_WINDOW_ARB 0x2001
		0x2010, true, // WGL_SUPPORT_OPENGL_ARB 0x2010
		0x2011, params.double_buffer, // WGL_DOUBLE_BUFFER_ARB 0x2011
		0x2003, 0x2027, // WGL_ACCELERATION_ARB 0x2003 WGL_FULL_ACCELERATION_ARB 0x2027
		0x2013, 0x202B, // WGL_PIXEL_TYPE_ARB 0x2013 WGL_TYPE_RGBA_ARB 0x202B
		0x2014, params.color_bits, // WGL_COLOR_BITS_ARB 0x2014
		0x2022, params.depth_bits, // WGL_DEPTH_BITS_ARB 0x2022
		0x201B, params.alpha_bits, // WGL_ALPHA_BITS_ARB 0x201B
		0x2023, params.stencil_bits, // WGL_STENCIL_BITS_ARB 0x2023
		0x2041, params.samples_buffers, // WGL_SAMPLE_BUFFERS_ARB 0x2041
		0x2042, params.samples, // WGL_SAMPLES_ARB 0x2042
		0
	};

	// Set context profile
	int context_profile = 0x00000001; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
	if(params.profile == ndm::GLContextProfile::COMPATIBILITY_PROFILE)
		context_profile = 0x00000002; // WGL_CONTEXT_COMPABILITY_PROFILE_BIT_ARB

	// Flags
	int flags = 0;
	if(params.debug_mode == true) 
		flags = flags | 0x0001;
	
	// Set context attributes array
	const int context_attributes[] =
	{
		0x2091, params.major_version, // WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
		0x2092, params.minor_version, // WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
		0x2094, flags, // WGL_CONTEXT_FLAGS_ARB 0x2094
		0x9126, context_profile, // WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
		0
	};

	int pixel_format = -1;
	unsigned int number_of_formats = 0;
	if (wglChoosePixelFormatARB(m_native_attributes.device_context, pixel_format_attributes, nullptr, 1, &pixel_format, &number_of_formats) == FALSE)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
	SecureZeroMemory(&pixel_format_descriptor, sizeof(PIXELFORMATDESCRIPTOR));
	if (DescribePixelFormat(m_native_attributes.device_context, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor) == 0)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if (SetPixelFormat(m_native_attributes.device_context, pixel_format, &pixel_format_descriptor) == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	m_native_attributes.gl_device_context = wglCreateContextAttribsARB(m_native_attributes.device_context, nullptr, context_attributes);
	if (m_native_attributes.gl_device_context == nullptr)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	if (wglMakeCurrent(m_native_attributes.device_context, m_native_attributes.gl_device_context) == FALSE)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::DeleteOpenGLContext() const
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Delete the GL context
	if (m_native_attributes.gl_device_context == nullptr)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	if (wglDeleteContext(m_native_attributes.gl_device_context) == FALSE)
		return ndm::NDMResult::NDM_OPENGL_ERROR;

	// Make the current context null
	wglMakeCurrent(nullptr, nullptr);

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::Unload()
{
	if (m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if(m_native_attributes.handle == nullptr)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if(m_events.closed == false)
		DestroyWindow(m_native_attributes.handle);
		
	m_loaded = false;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetTitle(const std::string_view title)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	// Set window title (unicode version or not)
	#ifdef UNICODE
	std::wstring new_title = std::wstring(title.begin(), title.end());
	#else
	std::string new_title = std::string(title);
	#endif

	if (SetWindowText(m_native_attributes.handle, new_title.c_str()) == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetVisible(const bool visible) 
{ 
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if(visible == true)
	{
		ShowWindow(m_native_attributes.handle, SW_SHOW);
	} else {
		ShowWindow(m_native_attributes.handle, SW_HIDE);
	}

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetX(const int x)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if (SetWindowPos(m_native_attributes.handle, nullptr, x, GetY(), 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetY(const int y)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if (SetWindowPos(m_native_attributes.handle, nullptr, GetX(), y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetWidth(const int width)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if (SetWindowPos(m_native_attributes.handle, nullptr, 0, 0, width, GetHeight(), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	return ndm::NDMResult::NDM_OK;
}

ndm::NDMResult ndm::Display::SetHeight(const int height)
{
	if(m_loaded == false)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	if (SetWindowPos(m_native_attributes.handle, nullptr, 0, 0, GetWidth(), height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
		return ndm::NDMResult::NDM_DISPLAY_ERROR;

	return ndm::NDMResult::NDM_OK;
}

int ndm::Display::GetX() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };

	if (GetWindowRect(m_native_attributes.handle, &rect) == FALSE)
		return -1;

	return rect.left;
}

int ndm::Display::GetY() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };

	if (GetWindowRect(m_native_attributes.handle, &rect) == FALSE)
		return -1;

	return rect.bottom;
}

int ndm::Display::GetWidth() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };

	if (GetWindowRect(m_native_attributes.handle, &rect) == FALSE)
		return -1;

	return rect.right - rect.left;
}

int ndm::Display::GetHeight() const
{
	if (m_loaded == false)
		return -1;

	RECT rect = { 0, 0, 0, 0 };

	if (GetWindowRect(m_native_attributes.handle, &rect) == FALSE)
		return -1;

	return rect.bottom - rect.top;
}

#endif