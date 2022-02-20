#include "Window.h"


void VE::Window::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void VE::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	//auto app = reinterpret_cast<vulkanEngine*>(glfwGetWindowUserPointer(window));
	//app->framebufferResized = true;
}
