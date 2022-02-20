#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

namespace VE {

	class Window {

		public:

			Window(uint32_t width, uint32_t height, const char* title) 
				: width{ width }, height{ height }, title{ title } {}

			void initWindow();
			void CreateSurface(VkInstance instance);
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

			GLFWwindow*		getWindow()  const { return window; }
			uint32_t	    getWidth()   const { return width; }
			uint32_t	    getHeight()  const { return height; }
			std::string		getTitle()   const { return title; }
			VkSurfaceKHR    getSurface() const { return surface; }

		private:
			uint32_t		width;
			uint32_t		height;
			const char*		title;
			GLFWwindow*		window;
			VkSurfaceKHR	surface;
			VkInstance		instance;
	};
}