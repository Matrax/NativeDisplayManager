// Only compile on Windows (x32 or x64)
#if defined(_WIN32) || defined(_WIN64)

// NDM includes
#include <ndm/opengl/gl_context.hpp>

void ndm::GLContext::load(const GLContextParams & params)
{
	if(m_display_ptr == nullptr)
		throw std::exception("There is no display bound to this GLContext !");

	if(m_display_ptr->is_loaded() == false)
		throw std::exception("The display is not loaded !");

	if(wglGetCurrentContext() != nullptr)
		throw std::exception("The current thread already has an OpenGL context !");

	// Fake window
	HDC fake_device_context = nullptr;
	HGLRC fake_gl_device_context = nullptr;
	HWND fake_handle = nullptr;

	// Register fake Win32 class
	WNDCLASSEXA fake_window_class = {};
	std::memset(&fake_window_class, 0, sizeof(WNDCLASSEXA));
	fake_window_class.cbSize = sizeof(WNDCLASSEXA);
	fake_window_class.lpszClassName = "FakeWindow";
	fake_window_class.lpfnWndProc = ndm::win32_process_events;
	fake_window_class.hInstance = m_display_ptr->get_win32_instance();
	fake_window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
	fake_window_class.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	if (RegisterClassExA(&fake_window_class) == 0)
		throw std::exception("Can't register the window class !");

	// Create the fake window
	fake_handle = CreateWindowExA(
		0, "FakeWindow", "FakeWindow",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 1, 1,
		nullptr, nullptr,
		m_display_ptr->get_win32_instance(), nullptr
	);

	// Get the fake device context
	fake_device_context = GetDC(fake_handle);
	if (fake_device_context == nullptr)
		throw std::exception("Can't retrieve the device context !");

	// Choose the fake pixel format
	PIXELFORMATDESCRIPTOR fake_pixel_format_descriptor = {};
	std::memset(&fake_pixel_format_descriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
	fake_pixel_format_descriptor.nVersion = 1;
	fake_pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	fake_pixel_format_descriptor.dwFlags = PFD_SUPPORT_OPENGL;
	fake_pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
	fake_pixel_format_descriptor.cColorBits = 8;
	int fake_pixel_format = ChoosePixelFormat(fake_device_context, &fake_pixel_format_descriptor);
	if (SetPixelFormat(fake_device_context, fake_pixel_format, &fake_pixel_format_descriptor) == FALSE)
		throw std::exception("Can't choose a pixel format !");

	// Create the fake GL Context
	fake_gl_device_context = wglCreateContext(fake_device_context);
	if (fake_gl_device_context == nullptr)
		throw std::exception("Can't create an OpenGL context !");

	// Set the fake OpenGL context active
	if (wglMakeCurrent(fake_device_context, fake_gl_device_context) == FALSE)
		throw std::exception("Can't make the current thread an OpenGL context !");

	// Load WGL functions
	ndm::wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
	ndm::wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	ndm::wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

	// Delete the fake GL context
	if (wglDeleteContext(fake_gl_device_context) == FALSE)
		throw std::exception("Can't delete the current OpenGL context !");

	// Remove the current fake OpenGL context
	wglMakeCurrent(nullptr, nullptr);

	// Destroy the fake window
	if (DestroyWindow(fake_handle) == false)
		throw std::exception("Can't destroy the window properly !");

	// Unregister the fake class
	if (UnregisterClassA("FakeWindow", m_display_ptr->get_win32_instance()) == 0)
		throw std::exception("can't unregister the window class !");

	// Choose pixel format
	const int pixel_format_attributes[] =
	{
		ndm::WGL_DRAW_TO_WINDOW_ARB, true,
		ndm::WGL_SUPPORT_OPENGL_ARB, true,
		ndm::WGL_DOUBLE_BUFFER_ARB, params.double_buffer,
		ndm::WGL_ACCELERATION_ARB, ndm::WGL_FULL_ACCELERATION_ARB,
		ndm::WGL_PIXEL_TYPE_ARB, ndm::WGL_TYPE_RGBA_ARB,
		ndm::WGL_COLOR_BITS_ARB, params.color_bits,
		ndm::WGL_DEPTH_BITS_ARB, params.depth_bits,
		ndm::WGL_ALPHA_BITS_ARB, params.alpha_bits,
		ndm::WGL_STENCIL_BITS_ARB, params.stencil_bits,
		ndm::WGL_SAMPLE_BUFFERS_ARB, params.samples_buffers,
		ndm::WGL_SAMPLES_ARB, params.samples,
		0
	};

	int pixel_format = -1;
	unsigned int number_of_formats = 0;
	if (ndm::wglChoosePixelFormatARB(m_display_ptr->get_win32_device_context(), pixel_format_attributes, nullptr, 1, &pixel_format, &number_of_formats) == FALSE)
		throw std::exception("Can't choose a pixel format with WGL !");

	PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
	std::memset(&pixel_format_descriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
	if (DescribePixelFormat(m_display_ptr->get_win32_device_context(), pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor) == 0)
		throw std::exception("Can't describe a pixel format !");

	if (SetPixelFormat(m_display_ptr->get_win32_device_context(), pixel_format, &pixel_format_descriptor) == false)
		throw std::exception("Can't set the pixel format !");

	// Set context attributes array
	int flags = 0;
	if(params.debug_mode == true) 
		flags = flags | 0x0001;

	int context_profile = ndm::WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	if(params.profile == ndm::GLContextProfile::COMPATIBILITY_PROFILE)
		context_profile = ndm::WGL_CONTEXT_COMPABILITY_PROFILE_BIT_ARB;

	const int context_attributes[] =
	{
		ndm::WGL_CONTEXT_MAJOR_VERSION_ARB, params.major_version,
		ndm::WGL_CONTEXT_MINOR_VERSION_ARB, params.minor_version,
		ndm::WGL_CONTEXT_FLAGS_ARB, flags,
		ndm::WGL_CONTEXT_PROFILE_MASK_ARB, context_profile,
		0
	};

	m_gl_device_context = ndm::wglCreateContextAttribsARB(m_display_ptr->get_win32_device_context(), nullptr, context_attributes);
	if (m_gl_device_context == nullptr)
		throw std::exception("Can't create an OpenGL context with WGL !");

	// Make current thread an OpenGL context
	if (wglMakeCurrent(m_display_ptr->get_win32_device_context(), m_gl_device_context) == FALSE)
		throw std::exception("Can't make the current thread an OpenGL context !");

	m_loaded = true;
}

void ndm::GLContext::unload()
{
	if(m_display_ptr == nullptr)
		throw std::exception("There is no display bound to this GLContext !");

	if(m_display_ptr->is_loaded() == false)
		throw std::exception("The display is not loaded !");

	if(m_loaded == false)
		throw std::exception("The OpenGL context is not loaded !");

	if(wglGetCurrentContext() == nullptr)
		throw std::exception("The current thread doesn't have an OpenGL context !");

	// Delete the GL context
	if (m_gl_device_context == nullptr)
		throw std::exception("Can't delete the OpenGL device context !");

	if (wglDeleteContext(m_gl_device_context) == FALSE)
		throw std::exception("Can't delete the current OpenGL context !");

	// Make the current context null
	wglMakeCurrent(nullptr, nullptr);

	m_loaded = false;
}

void ndm::GLContext::set_vertical_sync(const bool vertical_sync) const
{
	// Set the swap interval
	if (ndm::wglSwapIntervalEXT != nullptr && vertical_sync == true)
		ndm::wglSwapIntervalEXT(1);
	else if (ndm::wglSwapIntervalEXT != nullptr && vertical_sync == false)
		ndm::wglSwapIntervalEXT(0);
}

void ndm::GLContext::swap_front_and_back() const 
{
	if(m_display_ptr == nullptr)
		throw std::exception("There is no display bound to this GLContext !");

	if(m_display_ptr->is_loaded() == false)
		throw std::exception("The display is not loaded !");

	if(m_loaded == false)
		throw std::exception("The OpenGL context is not loaded !");

	if(m_display_ptr->get_win32_device_context() == nullptr)
		throw std::exception("There is no device context !");

	if(wglGetCurrentContext() == nullptr)
		throw std::exception("The current thread doesn't have an OpenGL context !");

	// Swap the back and front
	SwapBuffers(m_display_ptr->get_win32_device_context());
}

#endif