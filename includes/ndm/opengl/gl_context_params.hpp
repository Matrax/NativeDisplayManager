#pragma once

// STD includes
#include <ctype.h>

// NDM includes
#include <ndm/opengl/gl_context_profile.hpp>

namespace ndm
{
	/**
	* This structure describe the parameters to create of an OpenGL context for the current thread.
	*/
	struct GLContextParams
	{
		ndm::GLContextProfile profile;
		std::int32_t major_version;
		std::int32_t minor_version;
		std::int32_t color_bits;  	    
		std::int32_t alpha_bits; 
		std::int32_t depth_bits; 
		std::int32_t stencil_bits; 
		std::int32_t samples;
		bool debug_mode;
		bool double_buffer;
		bool samples_buffers;
	};
}