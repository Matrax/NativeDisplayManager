#pragma once

// Only compile on Windows (x32 or x64)
#if defined(_WIN32) or defined(_WIN64)

// STD includes
#include <iostream>
#include <string>
#include <string_view>
#include <exception>

// Windows API includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// WGL Extensions functions pointer and attributes
#ifdef W32_DISPLAY_OPENGL
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

namespace W32Display
{
	// W32DisplayEvent Struct -> manage all events of the window
	typedef struct W32DisplayEvent
	{
		bool resized;
		bool closed;
		bool minimized;
		bool maximized;
		bool moved;
		bool language_changed;
	} W32DisplayEvent;

	// W32Display class
	class W32Display
	{
		private:

			// Attributes
			#ifdef UNICODE
			std::wstring m_class_name;
			#else
			std::string m_class_name;
			#endif

			#ifdef W32_DISPLAY_OPENGL
			HGLRC m_gl_device_context;
			#endif

			HINSTANCE* m_instance;
			W32DisplayEvent m_events;
			MSG m_messages;
			HDC m_device_context;
			HWND m_handle;
			bool m_doublebuffered;
			bool m_loaded;

			// Static
			inline static W32Display * GLOBAL_INSTANCE = nullptr;

			#ifdef W32_DISPLAY_OPENGL
			inline static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
			inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
			inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
			#endif

			// Win32 events process function
			static LRESULT CALLBACK WindowProcessEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
			{
				W32Display * display = W32Display::GLOBAL_INSTANCE;

				if (display != nullptr)
				{
					switch (message)
					{
						case WM_DESTROY:
							display->m_events.closed = true;
							break;
						case WM_SIZE:
							if (wParam == SIZE_RESTORED) 
								display->m_events.resized = true;
							if (wParam == SIZE_MINIMIZED) 
								display->m_events.minimized = true;
							if (wParam == SIZE_MAXIMIZED) 
								display->m_events.maximized = true;
							break;
						case WM_MOVE:
							display->m_events.moved = true;
							break;
						case WM_INPUTLANGCHANGE:
							display->m_events.language_changed = true;
							break;
					}
				}

				return DefWindowProc(handle, message, wParam, lParam);
			}

			// Load WGL extensions needed to create the OpenGL context
			#ifdef W32_DISPLAY_OPENGL
			static void LoadWGLExtensions(HINSTANCE * instance)
			{
				// Fake window
				HDC fake_device_context = nullptr;
				HGLRC fake_gl_device_context = nullptr;
				HWND fake_handle = nullptr;

				// Register fake Win32 class
				WNDCLASSEX fake_window_class = {};
				SecureZeroMemory(&fake_window_class, sizeof(WNDCLASSEX));
				fake_window_class.lpszClassName = L"FakeWindow";
				fake_window_class.lpfnWndProc = W32Display::WindowProcessEvent;
				fake_window_class.hInstance = *(instance);
				fake_window_class.cbSize = sizeof(WNDCLASSEX);
				fake_window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
				fake_window_class.style = CS_OWNDC;

				if (RegisterClassEx(&fake_window_class) == 0)
					throw std::runtime_error("Error on register the fake windows class !");

				// Create the fake window
				fake_handle = CreateWindowEx(0, L"FakeWindow",
											 L"FakeWindow",
											 WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
											 0, 0,
											 1, 1,
											 nullptr, nullptr,
											 *(instance), nullptr);

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
				if (W32Display::W32Display::wglChoosePixelFormatARB == nullptr
					|| W32Display::W32Display::wglCreateContextAttribsARB == nullptr
					|| W32Display::W32Display::wglSwapIntervalEXT == nullptr)
				{
					W32Display::W32Display::wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
					if (*wglChoosePixelFormatARB == nullptr)
						throw std::runtime_error("Can't get the wglChoosePixelFormatARB function !");

					W32Display::W32Display::wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
					if (*wglCreateContextAttribsARB == nullptr)
						throw std::runtime_error("Can't get the wglCreateContextAttribsARB function !");

					W32Display::W32Display::wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
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
				if (UnregisterClass(L"FakeWindow", *(instance)) == 0)
					throw std::runtime_error("Error on unregister the fake window !");
			}
			#endif

		public:

			// No default constructor
			W32Display() = delete;

			// No copy constructor
			W32Display(W32Display&) = delete;

			W32Display(const W32Display&) = delete;

			// Constructor
			W32Display(HINSTANCE* instance) :
				m_events({}),
				m_loaded(false),
				m_handle(nullptr),
				m_device_context(nullptr),
				#ifdef W32_DSIPLAY_OPENGL
				m_gl_device_context(nullptr),
				#endif
				m_messages({}),
				m_doublebuffered(false),
				m_instance(instance)
			{
				if (W32Display::GLOBAL_INSTANCE != nullptr)
					throw std::runtime_error("Can only create one display per application !");

				// Check HINSTANCE
				if (instance == nullptr)
					throw std::runtime_error("Can't instantiate a Win32 Window with no HINSTANCE !");

				// Set the global instance
				W32Display::GLOBAL_INSTANCE = this;
			}

			// Virtual destructor
			virtual ~W32Display() 
			{
				if (m_loaded == true)
					Unload();

				W32Display::GLOBAL_INSTANCE = nullptr;
			}

			// Get the global instance
			static W32Display* GetInstance() 
			{ 
				return W32Display::GLOBAL_INSTANCE; 
			}

			// Load the window
			void Load(const std::string_view title, const int width, const int height, const bool visible = false)
			{
				if (m_loaded == true)
					throw std::runtime_error("Can't load a window that is already loaded !");

				// Set window class name (Unicode version or not)
				#ifdef UNICODE
				m_class_name = std::wstring(title.begin(), title.end());
				#else
				m_class_name = std::string(title));
				#endif

				// Register Win32 class
				WNDCLASSEX window_class = {};
				SecureZeroMemory(&window_class, sizeof(WNDCLASSEX));
				window_class.lpszClassName = m_class_name.c_str();
				window_class.lpfnWndProc = W32Display::WindowProcessEvent;
				window_class.hInstance = *(m_instance);
				window_class.cbSize = sizeof(WNDCLASSEX);
				window_class.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
				window_class.style = CS_OWNDC;
				window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
				window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);

				if (RegisterClassEx(&window_class) == 0)
					throw std::runtime_error("Error on register the windows class !");

				// Create the window
				this->m_handle = CreateWindowEx(0, m_class_name.c_str(), m_class_name.c_str(),
												WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
												width, height,
												nullptr, nullptr, *(m_instance), nullptr);

				if (this->m_handle == nullptr)
					throw std::runtime_error("Error on create the window !");

				// Get the device context
				this->m_device_context = GetDC(m_handle);
				if (this->m_device_context == nullptr)
					throw std::runtime_error("Can't get the device context !");

				// If visible
				if (visible == true)
					this->Show();

				this->m_loaded = true;
			}

			// Get the events of the window
			W32DisplayEvent& GetEvents() noexcept
			{
				// Clear events
				SecureZeroMemory(&this->m_events, sizeof(W32DisplayEvent));

				// Clear messages
				SecureZeroMemory(&this->m_messages, sizeof(MSG));

				// While there are windows messages, we dipatch them
				while (PeekMessage(&this->m_messages, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&this->m_messages);
					DispatchMessage(&this->m_messages);
				}

				return this->m_events;
			}

			void SwapFrontAndBack(const int swap_interval) const noexcept
			{
				if (m_loaded == true && m_device_context != nullptr)
				{
					// Swap the back and front
					#ifdef W32_DISPLAY_OPENGL
					if (W32Display::W32Display::wglSwapIntervalEXT != nullptr)
						W32Display::W32Display::wglSwapIntervalEXT(swap_interval);
					wglSwapLayerBuffers(m_device_context, WGL_SWAP_MAIN_PLANE);
					#else
					SwapBuffers(m_device_context);
					#endif

				}
			}

			void Unload()
			{
				if (m_loaded == false)
					throw std::runtime_error("Can't unload a window that is already unloaded !");

				// Unregister class
				if (UnregisterClass(m_class_name.c_str(), *(m_instance)) == 0)
					throw std::runtime_error("Error on unregister the window !");

				m_loaded = false;
			}

			#ifdef W32_DISPLAY_OPENGL
			void MakeOldOpenGLContext(const bool double_buffer, const int color_bits, const int depth_bits, const int stencil_bits)
			{
				// Double buffered ?
				m_doublebuffered = double_buffer;

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
			#endif

			#ifdef W32_DISPLAY_OPENGL
			void MakeOpenGLContext(const int major_version, const int minor_version, const bool double_buffer,
								   const int color_bits, const int alpha_bits, const int depth_bits, const int stencil_bits,
								   const bool samples_buffers, const int samples)
			{
				// Double buffered ?
				m_doublebuffered = double_buffer;

				// Get the WGL functions
				W32Display::LoadWGLExtensions(this->m_instance);
				if (W32Display::W32Display::wglChoosePixelFormatARB == nullptr || 
					W32Display::W32Display::wglCreateContextAttribsARB == nullptr ||
					W32Display::W32Display::wglSwapIntervalEXT == nullptr)
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
					0x201B, alpha_bits, // WGL_ALPHA_BITS_ARB 0x201B
					0x2022, depth_bits, // WGL_DEPTH_BITS_ARB 0x2022
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
				if (W32Display::W32Display::wglChoosePixelFormatARB(this->m_device_context, pixel_format_attributes, nullptr, 1, &pixel_format, &number_of_formats) == FALSE)
					throw std::runtime_error("Can't set the pixel format !");

				PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
				SecureZeroMemory(&pixel_format_descriptor, sizeof(PIXELFORMATDESCRIPTOR));

				if (DescribePixelFormat(this->m_device_context, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor) == 0)
					throw std::runtime_error("Can't describe the pixel format !");

				if (SetPixelFormat(this->m_device_context, pixel_format, &pixel_format_descriptor) == false)
					throw std::runtime_error("Can't set the pixel format !");

				this->m_gl_device_context = W32Display::W32Display::wglCreateContextAttribsARB(this->m_device_context, nullptr, context_attributes);
				if (this->m_gl_device_context == nullptr)
					throw std::runtime_error("Can't create the OpenGL context !");

				if (wglMakeCurrent(this->m_device_context, this->m_gl_device_context) == FALSE)
					throw std::runtime_error("Can't make the OpenGL context !");
			}
			#endif

			#ifdef W32_DISPLAY_OPENGL
			void DeleteOpenGLContext() const
			{
				// Delete the GL context
				if (this->m_gl_device_context == nullptr)
					throw std::runtime_error("There is no OpenGL context active !");

				if (wglDeleteContext(this->m_gl_device_context) == FALSE)
					throw std::runtime_error("Can't delete the OpenGL Context");

				// Make the current context null
				wglMakeCurrent(nullptr, nullptr);
			}
			#endif

			void SetTitle(const std::string_view title)
			{
				// Set window title (unicode version or not)
				#ifdef UNICODE
				std::wstring new_title = std::wstring(title.begin(), title.end());
				#else
				std::string new_title = std::string(title);
				#endif

				if (SetWindowText(this->m_handle, new_title.c_str()) == false)
					throw std::runtime_error("Can't set the title of the window !");
			}

			void SetX(const int x)
			{
				if (SetWindowPos(this->m_handle, nullptr, x, this->GetY(), 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
					throw std::runtime_error("Can't move the window !");
			}

			void SetY(const int y)
			{
				if (SetWindowPos(this->m_handle, nullptr, this->GetX(), y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
					throw std::runtime_error("Can't move the window !");
			}

			void SetWidth(const int width)
			{
				if (SetWindowPos(this->m_handle, nullptr, 0, 0, width, this->GetHeight(), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
					throw std::runtime_error("Can't move the window !");
			}

			void SetHeight(const int height)
			{
				if (SetWindowPos(this->m_handle, nullptr, 0, 0, this->GetWidth(), height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER) == FALSE)
					throw std::runtime_error("Can't move the window !");
			}

			int GetX() const
			{
				if (this->m_loaded == false)
					throw std::runtime_error("Can't get the y position because the window is unloaded !");

				RECT rect = { 0, 0, 0, 0 };

				if (GetWindowRect(this->m_handle, &rect) == FALSE)
					throw std::runtime_error("Can't get the width of the window !");

				return rect.left;
			}

			int GetY() const
			{
				if (this->m_loaded == false)
					throw std::runtime_error("Can't get the x position because the window is unloaded !");

				RECT rect = { 0, 0, 0, 0 };

				if (GetWindowRect(this->m_handle, &rect) == FALSE)
					throw std::runtime_error("Can't get the height of the window !");

				return rect.bottom;
			}

			int GetWidth() const
			{
				if (this->m_loaded == false)
					throw std::runtime_error("Can't get the width because the window is unloaded !");

				RECT rect = { 0, 0, 0, 0 };

				if (GetWindowRect(this->m_handle, &rect) == FALSE)
					throw std::runtime_error("Can't get the width of the window !");

				return rect.right - rect.left;
			}

			int GetHeight() const
			{
				if (this->m_loaded == false)
					throw std::runtime_error("Can't get the height because the window is unloaded !");

				RECT rect = { 0, 0, 0, 0 };

				if (GetWindowRect(this->m_handle, &rect) == FALSE)
					throw std::runtime_error("Can't get the height of the window !");

				return rect.bottom - rect.top;
			}

			inline HWND GetWin32Handler() const noexcept 
			{ 
				return this->m_handle; 
			}

			inline HDC GetDeviceContext() const noexcept 
			{ 
				return this->m_device_context; 
			}

			#ifdef W32_DISPLAY_OPENGL
			inline HGLRC GetGLDeviceContext() const noexcept 
			{ 
				return this->m_gl_device_context; 
			}
			#endif

			inline void Close() const noexcept 
			{ 
				DestroyWindow(this->m_handle); 
			}

			inline void Show() const noexcept 
			{ 
				ShowWindow(this->m_handle, SW_SHOW); 
			}

			inline void Hide() const noexcept 
			{ 
				ShowWindow(this->m_handle, SW_HIDE); 
			}

			inline bool IsDoubleBufferedContext() const noexcept 
			{ 
				return this->m_doublebuffered; 
			}
	};
};

#endif