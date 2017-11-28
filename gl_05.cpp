#define GLEW_STATIC
#include <GL/glew.h>
#include "shprogram.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 800, HEIGHT = 600;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraDU = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
		throw exception("Failed to load texture file");

	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

ostream& operator<<(ostream& os, const glm::mat4& mx)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
			cout << mx[row][col] << ' ';
		cout << endl;
	}
	return os;
}

int main()
{
	//{
		//glm::mat4 trans;
		//cout << trans << endl;
		//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		//cout << trans << endl;
	//}
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try
	{
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GKOM - OpenGL 05", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);

		// Let's check what are maximum parameters counts
		GLint nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		//cout << "Max vertex attributes allowed: " << nrAttributes << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &nrAttributes);
		//cout << "Max texture coords allowed: " << nrAttributes << std::endl;

		// Build, compile and link shader program
		ShaderProgram theProgram("gl_05.vert", "gl_05.frag");
		ShaderProgram theProgramB("gl_05_B.vert", "gl_05_B.frag");
		ShaderProgram theProgramC("gl_05_C.vert", "gl_05_C.frag");
		
		// Set up vertex data 
		float vertices[] = {
			-0.5f, -2.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.5f, -2.0f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -2.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

			-0.5f, -2.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.5f, -2.0f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -2.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -2.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			-0.5f, -2.0f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			-0.5f, -2.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			 
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.5f, -2.0f, -0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.5f, -2.0f, -0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.5f, -2.0f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f, 0.0f, 0.0f,

			-0.5f, -2.0f, -0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,
			0.5f, -2.0f, -0.5f,  1.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.5f, -2.0f,  0.5f,  1.0f, 0.0f,0.0f, 1.0f, 1.0f,
			0.5f, -2.0f,  0.5f,  1.0f, 0.0f,0.0f, 1.0f, 1.0f,
			-0.5f, -2.0f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, 1.0f,
			-0.5f, -2.0f, -0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f, 1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,
		};

		float verticesB[] = {
			0.3f,  0.0f, 0.6f,  0.0f, 0.0f,0.0f, 0.0f, 0.0f,
			0.7f,  0.0f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.0f,  0.3f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			1.0f,  0.3f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.4f,  0.4f, 0.6f,  0.0f, 1.0f,0.0f, 0.0f, 1.0f,
			0.6f,  0.4f, 0.6f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,

			0.4f,  0.6f,  0.6f,  0.0f, 0.0f,0.0f, 0.0f, 0.0f,
			0.6f,  0.6f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.0f,  0.7f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			1.0f,  0.7f, 0.6f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.3f,  1.0f, 0.6f,  0.0f, 1.0f,0.0f, 0.0f, 1.0f,
			0.7f,  1.0f, 0.6f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,

			0.3f,  0.0f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, 0.0f,
			0.7f,  0.0f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.0f,  0.3f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			1.0f,  0.3f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.4f,  0.4f, 0.5f,  0.0f, 1.0f,0.0f, 0.0f, 1.0f,
			0.6f,  0.4f, 0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,

			0.4f,  0.6f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, 0.0f,
			0.6f,  0.6f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 0.0f,
			0.0f,  0.7f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			1.0f,  0.7f, 0.5f,  0.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.3f,  1.0f, 0.5f,  0.0f, 1.0f,0.0f, 0.0f, 1.0f,
			0.7f,  1.0f, 0.5f,  0.0f, 1.0f,0.0f, 0.0f, 0.0f,
		};
		int a = 12;
		GLuint indicesB[] = {
			0, 1, 4,
			4,1,5,
			5,3,7,
			7,3,9,
			7,10,11,
			10,7,6,
			6,2,8,
			2,6,4,
			5,4,6,
			6,5,7,

			0+a, 1+a, 4 + a,
			4 + a,1 + a,5 + a,
			5 + a,3 + a,7 + a,
			7 + a,3 + a,9 + a,
			7 + a,10 + a,11 + a,
			10 + a,7 + a,6 + a,
			6 + a,2 + a,8 + a,
			2 + a,6 + a,4 + a,
			5 + a,4 + a,6 + a,
			6 + a,5 + a,7 + a,

			3, 15,9,
			9,15,21,
			9,7,19,
			19,9,21,
			7,19,11,
			11,19,23,
			23,22,11,
			11,22,10,
			10,22,18,
			18,10,6,
			6,8,20,
			20,6,18,
			8,2,14,
			14,20,8,
			2,4,14,
			14,4,16,
			16,4,12,
			12,4,0,
			0,1,12,
			12,1,13,
			1,13,5,
			5,13,17,
			5,17,3,
			3,17,15
		};
		
		int num_segments = 20;
		
		int indeks = 0;
		float x, y;
		bool xUP = true;
		float r = 0.5;
		x = 0; y = 0;
		float* cylinderVertices = new float[num_segments * 5 * 2];
		GLuint* cylinderIndices = new GLuint[3 * (2*(num_segments-1)+2*(num_segments+1)) ];
		
		float theta;
		int pom = 0;
		for (float k = 0; k < 2; k = k + 1) {
			for (int i = 0; i < num_segments; i++) {
				theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
				x = r * cosf(theta);
				y = r * sinf(theta);
				cylinderVertices[indeks] = x;indeks++;
				cylinderVertices[indeks] = y; indeks++;
				cylinderVertices[indeks] = k/5; indeks++;
				if (pom == 0) {
					cylinderVertices[indeks] = 0.0f; indeks++;
					cylinderVertices[indeks] = 1.0f; indeks++;
				}
				if (pom == 1) {
					cylinderVertices[indeks] = 1.0f; indeks++;
					cylinderVertices[indeks] = 0.0f; indeks++;
				}
				if (pom == 2) {
					cylinderVertices[indeks] = 0.0f; indeks++;
					cylinderVertices[indeks] = 0.0f; indeks++;
				}
				if (pom == 3) {
					cylinderVertices[indeks] = 1.0f; indeks++;
					cylinderVertices[indeks] = 1.0f; indeks++;
				}
				pom++;
				if (pom == 4) pom = 0;
			}
		}
		
		indeks = 0;
		int indeksI = 0;
		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < num_segments - 1; i++) {
				cylinderIndices[indeksI] = GLuint(0 + num_segments*k); indeks++; indeksI++;
				cylinderIndices[indeksI] = GLuint(indeks); indeks++; indeksI++;
				cylinderIndices[indeksI] = GLuint(indeks); indeks--; indeksI++;
			}
		}
		indeks = 0;
		for (int i = 0; i < num_segments - 1; i++) {
			cylinderIndices[indeksI] = indeks; indeksI++;
			cylinderIndices[indeksI] = indeks + 1;indeksI++;
			cylinderIndices[indeksI] = indeks + num_segments; indeksI++;
			cylinderIndices[indeksI] = indeks + 1; indeksI++;
			cylinderIndices[indeksI] = indeks + num_segments; indeksI++;
			cylinderIndices[indeksI] = indeks + num_segments + 1; indeksI++;
			indeks++;
		}
		cylinderIndices[indeksI] = num_segments-1; indeksI++;
		cylinderIndices[indeksI] = num_segments; indeksI++;
		cylinderIndices[indeksI] = 2 * num_segments -1; indeksI++;

		GLuint VBO, EBO, VAO, VAOB, VBOB, EBOB, VAOC, VBOC, EBOC;
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glGenVertexArrays(1, &VAOB);
		glGenBuffers(1, &VBOB);
		glGenBuffers(1, &EBOB);
		
		glGenVertexArrays(1, &VAOC);
		glGenBuffers(1, &VBOC);
		glGenBuffers(1, &EBOC);

		glEnable(GL_DEPTH_TEST);

		// pierwszy obiekt; // wiatrak
		glBindVertexArray(VAOB);
		glBindBuffer(GL_ARRAY_BUFFER, VBOB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB), verticesB, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesB), indicesB, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		glBindVertexArray(0);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		
		// drugi obiekt // domek

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
		 
		// trzeci obiekt 

		glBindVertexArray(VAOC);
		glBindBuffer(GL_ARRAY_BUFFER, VBOC);
		glBufferData(GL_ARRAY_BUFFER, 4*num_segments * 5 * 2, cylinderVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * 3 * (2 * (num_segments - 1) + 2 * (num_segments + 1)), cylinderIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		
		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		glBindVertexArray(0);

							  // Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// prepare textures
		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "deski.jpg");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "skrzydlo.jpg");
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "kamien.jpg");

		float cameraSpeed = 0.005f; // adjust accordingly
		static GLfloat rot_angle = 0.0f;

		// main event loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Clear the colorbuffer
			glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			processInput(window);

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			theProgram.Use();
			// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture0"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture1"), 1);

			glm::mat4 trans;
			GLuint transformLoc = glGetUniformLocation(theProgram.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

			glm::mat4 view;
			glm::mat4 projection;
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(glm::radians(45.0f),(GLfloat)WIDTH / (GLfloat)HEIGHT,0.1f, 100.0f);
			GLint viewLoc = glGetUniformLocation(theProgram.get_programID(), "view");
			GLint projLoc = glGetUniformLocation(theProgram.get_programID(),"projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			
			glDepthFunc(GL_LEQUAL);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
			
			// drugi obiekt

			theProgramB.Use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(theProgramB.get_programID(), "Texture0"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgramB.get_programID(), "Texture1"), 1);

			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0, 0, 1));
			trans = glm::translate(trans, glm::vec3(-0.5, -0.5, 0.01));
			rot_angle += 0.05f;
			if (rot_angle >= 360.0f)
				rot_angle -= 360.0f;
			transformLoc = glGetUniformLocation(theProgramB.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			viewLoc = glGetUniformLocation(theProgramB.get_programID(), "view");
			projLoc = glGetUniformLocation(theProgramB.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAOB);
			glDrawElements(GL_TRIANGLES, _countof(indicesB), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// trzeci obiekt

			theProgramC.Use();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(theProgramC.get_programID(), "Texture2"), 2);

			glm::mat4 trans2;
			trans2 = glm::rotate(trans2, glm::radians(90.0f), glm::vec3(1, 0, 0));
			trans2 = glm::translate(trans2, glm::vec3(0, 2, 1.75));
			trans2 = glm::rotate(trans2, -glm::radians(rot_angle), glm::vec3(0, 0, 1));
		//	rot_angle += 0.05f;
			//if (rot_angle >= 360.0f)
				//rot_angle -= 360.0f;
			transformLoc = glGetUniformLocation(theProgramC.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			viewLoc = glGetUniformLocation(theProgramC.get_programID(), "view");
			projLoc = glGetUniformLocation(theProgramC.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAOC);
			glDrawElements(GL_TRIANGLES, 4 * 3 * (2 * (num_segments - 2) + 2 * num_segments), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Swap the screen buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAOB);
		glDeleteBuffers(1, &VBOB);
		glDeleteBuffers(1, &EBOB);
		glDeleteVertexArrays(1, &VAOC);
		glDeleteBuffers(1, &VBOC);
		glDeleteBuffers(1, &EBOC);
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	glfwTerminate();

	return 0;
}
