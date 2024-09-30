#pragma once

namespace ndm
{
	/**
	* This structure represent all the events happening during the execution by a display, getters for all 
	* of them are defined in the display class directly, all implementations must use this structure to report 
	* all the following events :
	* - display resized, minimized, maximized, moved, closed, language changed
	*/
	struct DisplayEvents
	{
		bool resized;
		bool closed;
		bool minimized;
		bool maximized;
		bool moved;
	};
}