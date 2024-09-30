#pragma once

namespace ndm
{
	/*
	* Enumeration that represent the profile used for the creation of an OpenGL context.
	* There are two types of context : core and compability.
	*/
	enum class GLContextProfile
	{
		CORE_PROFILE,
		COMPATIBILITY_PROFILE
	};
}