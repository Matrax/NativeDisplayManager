#pragma once

namespace NativeDisplayManager
{
	/*
	* This structure represent all the events happening during the execution by a display, getters for all 
	* of them are defined in the display class directly, all implementations must use this structure to report 
	* all the following events :
	* - mouse and keyboard inputs 
	* - mouse position and mouse direction
	* - display resized, minimized, maximized, moved
	* - display closed
	* - os languages changed
	*/
	struct DisplayEvents
	{
		int m_keys_down[32];
		int m_keys_up[32];
		int previous_mouse_x;
		int previous_mouse_y;
		int mouse_x;
		int mouse_y;
		int mouse_direction_x;
		int mouse_direction_y;
		bool resized;
		bool closed;
		bool minimized;
		bool maximized;
		bool moved;
		bool moused_moved;
		bool language_changed;
		bool left_mouse_pressed;
		bool right_mouse_pressed;
		bool middle_mouse_pressed;
		bool left_mouse_released;
		bool right_mouse_released;
		bool middle_mouse_released;
	};
}

