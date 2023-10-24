#pragma once

namespace ndm
{
	/*
	* Enumeration that represent all the possible results from all the functions of the library.
	*/
	enum class NDMResult
	{
		NDM_DISPLAY_ERROR,
		NDM_OPENGL_ERROR,
        NDM_VULKAN_ERROR,
        NDM_DIRECTX_ERROR,
		NDM_MONITOR_ERROR,
		NDM_OK
	};
}