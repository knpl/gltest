#include <iostream>
#include <exception>
#include <Windows.h>

// GLFW header should be last (It detects the presence of other headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define WINDOW_TITLE "New window"

void error_callback(int error, const char *message) {
	std::cerr << message;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void start(GLFWwindow *window) {
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void initGlfw() {
	try {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize glfw");
		}

		glfwSetErrorCallback(error_callback);

		
		// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		

		GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
		if (!window) {
			throw std::runtime_error("Failed to create window");
		}
		try {
			glfwMakeContextCurrent(window);
			glfwSetKeyCallback(window, key_callback);

			if (glewInit() != GLEW_OK) {
				throw std::runtime_error("Failed to initialize glew");
			}

			start(window);

			glfwDestroyWindow(window);
		}
		catch (std::runtime_error &ex) {
			glfwDestroyWindow(window);
			throw ex;
		}

		glfwTerminate();
	}
	catch (std::runtime_error &ex) {
		std::cerr << "Message: " << ex.what();
		glfwTerminate();
	}
}

int main(int argc, char **argv) {
	initGlfw();
	char c;
	std::cout << "Enter a key...";
	std::cin >> c;
}