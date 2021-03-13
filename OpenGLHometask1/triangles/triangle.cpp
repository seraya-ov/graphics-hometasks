// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Triangles", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint firstProgramID = LoadShaders("VertexShader.vertexshader", "FirstFragmentShader.fragmentshader");
	GLuint secondProgramID = LoadShaders("VertexShader.vertexshader", "SecondFragmentShader.fragmentshader");
	float rotationAngle = .0f;


	static const GLfloat g_vertex_buffer_data_triangle[] = {
		0.6f, 0.0f, 0.0f,
		 0.0f, 0.4f, 0.0f,
		 0.0f,  0.0f, 0.4f,
		 0.6f, 0.0f, 0.0f,
		0.0f, 0.3f, 0.0f,
		0.0f,  0.0f, 0.6f,
	};
 

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_triangle), g_vertex_buffer_data_triangle, GL_STATIC_DRAW);

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ModelMatrix = glm::mat4(1.0);

	GLuint FirstMatrixID = glGetUniformLocation(firstProgramID, "MVP");
	GLuint SecondMatrixID = glGetUniformLocation(secondProgramID, "MVP");

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 View = glm::lookAt(
			glm::vec3(cos(rotationAngle), 0, sin(rotationAngle)),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		glm::mat4 MVP = ProjectionMatrix * View * ModelMatrix;

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangles !
		glUseProgram(firstProgramID);
		glUniformMatrix4fv(FirstMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(secondProgramID);
		glUniformMatrix4fv(SecondMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		rotationAngle += 0.002f;

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(firstProgramID);
	glDeleteProgram(secondProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}