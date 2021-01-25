#pragma once

#include "base.h"

#include <array>

#include <GLFW/glfw3.h>


class renderer {
public:

	// actual renderer
	class i_drawer
	{
	public:

		virtual void draw_frame(renderer& r) = 0;

	};

	// user input event handler
	class i_handler {

	public:

		virtual void handle_input(renderer& r) = 0;

	};

	renderer(int winWidth, int winHeight);
	~renderer();

	void main_loop(i_drawer& iDrawer, i_handler& iHandler);

	// 0 to disable vsync, 1 to enable
	void set_vsync_interval(int newInterval);

	// window title
	void set_title(const char* newTitle);

	bool get_key(int glfwKey);

protected:

	GLFWwindow* glWindow;

	std::array< bool, GLFW_KEY_LAST > btnReleased;

};