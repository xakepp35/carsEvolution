#include "renderer.h"

#pragma comment(lib, "OpenGL32.lib")

static const char DefaultRendererTitle[] = "GLFW3 renderer engine (c) xakepp35";

renderer::renderer(int winWidth, int winHeight):
	glWindow(nullptr),
	btnReleased({ false })
{
	CHK_EX(!glfwInit());
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	CHK_PTR(glWindow = glfwCreateWindow(winWidth, winHeight, DefaultRendererTitle, NULL, NULL));
	glfwMakeContextCurrent(glWindow);
	//glfwSetKeyCallback()
}


renderer::~renderer() {
	glfwMakeContextCurrent(nullptr);
	if(glWindow)
		glfwDestroyWindow(glWindow);
	glfwTerminate();
}


void renderer::main_loop(i_drawer& iDrawer, i_handler& iHandler) {
	while (!glfwWindowShouldClose(glWindow)) {
		iDrawer.draw_frame(*this);
		glfwSwapBuffers(glWindow);

		glfwPollEvents();
		iHandler.handle_input(*this);
	}
}


void renderer::set_vsync_interval(int newInterval) {
	int enableVSync = 0;
	glfwSwapInterval(enableVSync);
}


bool renderer::get_key(int glfwKey) {
	if ((glfwGetKey(glWindow, glfwKey) == GLFW_PRESS)) {
		if (btnReleased[glfwKey]) {
			btnReleased[glfwKey] = false;
			return true;
		}
	}
	else
		btnReleased[glfwKey] = true;
	return false;
}


void renderer::set_title(const char* newTitle) {
	glfwSetWindowTitle(glWindow, newTitle);
}