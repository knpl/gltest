#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>

#include <vector>
// GLFW header should be last (It detects the presence of other headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define WINDOW_TITLE "New window"

#pragma comment(lib, "glew32.lib")

void error_callback(int error, const char *message) {
	std::cerr << message;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

std::string get_shader_code(std::string const &path) {
	std::ifstream in_stream(path, std::ios::in);
	if (!in_stream.is_open()) {
		std::stringstream strstr;
		strstr << "Failed to open file with path: " << path;
		throw std::runtime_error(strstr.str());
	}

	std::stringstream strstr;
	strstr << in_stream.rdbuf();
	std::string code = strstr.str();
	in_stream.close();

	return code;
}

void compile_shader(GLuint shader_id, std::string const& shader_code) {

	char const* shader_code_ptr = shader_code.c_str();
	glShaderSource(shader_id, 1, &shader_code_ptr, NULL);
	glCompileShader(shader_id);

	GLint result;
	int info_log_length;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		std::stringstream strstr;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

		std::vector<char> buffer(info_log_length + 1);
		glGetShaderInfoLog(shader_id, info_log_length, NULL, &buffer[0]);
		buffer[info_log_length] = '\0';

		strstr << "Compilation failed: " << std::string(buffer.begin(), buffer.end());
		throw std::runtime_error(strstr.str());
	}
}

void link_shader_program(GLuint program_id) {
	glLinkProgram(program_id);

	GLint result;
	int info_log_length;
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	
	if (result == GL_FALSE) {
		std::stringstream strstr;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

		std::vector<char> buffer(info_log_length + 1);
		glGetProgramInfoLog(program_id, info_log_length, NULL, &buffer[0]);
		buffer[info_log_length] = '\0';

		strstr << "Linking failed: " << std::string(buffer.begin(), buffer.end());
		throw std::runtime_error(strstr.str());
	}
}

GLuint loadShaderProgram(std::string const& vertex_shader_path, std::string const& fragment_shader_path) {
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vs_code = get_shader_code(vertex_shader_path);
	std::string fs_code = get_shader_code(fragment_shader_path);

	compile_shader(vs, vs_code);
	compile_shader(fs, fs_code);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);

	// clean up shader objects.
	glDetachShader(prog, vs);
	glDetachShader(prog, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return prog;
}

void start(GLFWwindow *window) {
	glfwSwapInterval(1);

	glClearColor(0., 0., 0.4, 0.);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float buffer_data[] = {
		-1., -1., 0.,
		1., -1., 0.,
		0., 1., 0.
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), buffer_data, GL_STATIC_DRAW);

	GLuint prog = loadShaderProgram(
		"C:\\Users\\rene\\source\\repos\\glfw_test\\glfw_test\\vertex.vs", 
		"C:\\Users\\rene\\source\\repos\\glfw_test\\glfw_test\\fragment.fs");

	glm::mat4 scale(1.0f);
	glm::mat4 rotation(1.0f); // identity. No rotation yet.
	glm::mat4 translate(1.0f); // identity. No translation yet.

	glm::mat4 model = translate * rotation * scale;

	glm::vec3 cam_pos(0.0f, 0.0f, 3.0f);
	glm::vec3 look_at(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::lookAt(cam_pos, look_at, up);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f);

	glm::mat4 mvp = projection * view * model;

	GLuint mvp_uni_loc = glGetUniformLocation(prog, "mvp");

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(prog);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


		auto now = std::chrono::system_clock::now();
		long millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		double the = (millis_since_epoch / 1000.0) * 2.0 * M_PI;

		cam_pos = glm::vec3(3.0 * cos(the), 0.0, 3.0 * sin(the));
		look_at = glm::vec3(0.0, 0.0, 0.0);
		up = glm::vec3(0.0, 1.0, 0.0);
		view = glm::lookAt(cam_pos, look_at, up);
		mvp = projection * view * model;

		glUniformMatrix4fv(mvp_uni_loc, 1, GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);

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

		glfwWindowHint(GLFW_SAMPLES, 4); // 4x anti-aliasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		

		GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
		if (!window) {
			throw std::runtime_error("Failed to create window");
		}
		try {
			glfwMakeContextCurrent(window);
			glfwSetKeyCallback(window, key_callback);

			glewExperimental = true;

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
		std::cerr << "Message: " << ex.what() << std::endl;
		glfwTerminate();
	}
}

int main(int argc, char **argv) {
	initGlfw();
	char c;
	std::cout << "Enter a key..." << std::endl;
	std::cin >> c;
}