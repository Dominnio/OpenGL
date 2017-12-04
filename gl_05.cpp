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
#include "my_gl.h"
#include <ctime>
#include <cstdlib>

glm::vec3 lightPos(0.0f, 2.0f, 1.0f);

int main()
{
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

		// Build, compile and link shader program
		ShaderProgram theProgram("gl_05.vert", "gl_05.frag");
		ShaderProgram theProgramLight("gl_05_light.vert", "gl_05_light.frag");

		// Set up vertex data 
		float grassVertices[]{
			-10.0f, -0.51f, -10.0f, 0.0f,  1.0f, 0.0f,  0.0f,   0.0f,
			 10.0f, -0.51f, -10.0f, 0.0f,  1.0f, 0.0f, 50.0f,   0.0f,
			 10.0f, -0.51f,  10.0f, 0.0f,  1.0f, 0.0f, 50.0f,  50.0f,
			 10.0f, -0.51f,  10.0f, 0.0f,  1.0f, 0.0f, 50.0f,  50.0f,
			-10.0f, -0.51f,  10.0f, 0.0f,  1.0f, 0.0f,  0.0f,  50.0f,
			-10.0f, -0.51f, -10.0f, 0.0f,  1.0f, 0.0f,  0.0f,   0.0f,
		};
		float roofVertices[]{
			0.5f,  0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 0.0f,  1.0f,
			0.5f,  0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,
			0.0f,  1.0f,  0.5f, 0.5f,  0.0f, 0.0f, 0.0f,  0.0f,
			0.0f,  1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,
			0.0f,  1.0f,  0.5f, 0.5f,  0.0f, 0.0f, 0.0f,  0.0f,
			0.5f,  0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,
			 0.0f,  1.0f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f,
			 0.0f,  1.0f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f,
			 0.0f,  1.0f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f,
		};
		float szerokosc = 1, wysokosc = 1, dlugosc = 1;
		float s = szerokosc/2, w = wysokosc/2, d = dlugosc/2;
		float buldingVertices[] = {
			// positions          // normals           // texture coords
			-s, -w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			s, -w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			s,  w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			s,  w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-s,  w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-s, -w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-s, -w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			s, -w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			s,  w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			s, w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-s, w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-s, -w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-s, w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-s, w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-s, -w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-s, -w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-s, -w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-s, w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

			s, w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			s, w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			s, -w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			s, -w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			s, -w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			s, w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

			-s, -w, -d,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			s, -w, -d,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			s, -w,  d,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			s, -w,  d,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-s, -w,  d,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-s, -w, -d,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-s, w, -d,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			s, w, -d,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			s, w,  d,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			s, w,  d,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-s, w,  d,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-s, w, -d,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

			 s, w,  d,  0.0f,  0.0f, 1.0f,  1.0f,  0.0f,
			-s, w,  d,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
			 0.0f,  1.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,
			 s, w, -d,  0.0f,  0.0f, -1.0f, 1.0f,  0.0f,
			-s, w, -d,  0.0f,  0.0f, -1.0f, 0.0f,  0.0f,
			0.0f,  1.0f, -0.5f, 0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
		};
		szerokosc = 0.05, wysokosc = 1.0, dlugosc = 0.05;
		s = szerokosc / 2; w = wysokosc / 2; d = dlugosc / 2;
		float connectVertices[] = {
			// positions          // normals           // texture coords
			-s, -w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			s, -w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			s,  w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			s,  w, -d,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-s,  w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-s, -w, -d,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-s, -w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			s, -w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			s,  w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			s, w,  d,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-s, w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-s, -w,  d,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-s, w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-s, w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-s, -w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-s, -w, -d, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-s, -w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-s, w,  d, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

			s, w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			s, w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			s, -w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			s, -w, -d,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			s, -w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			s, w,  d,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

			-s, -w, -d,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			s, -w, -d,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			s, -w,  d,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			s, -w,  d,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-s, -w,  d,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-s, -w, -d,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-s, w, -d,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			s, w, -d,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			s, w,  d,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			s, w,  d,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-s, w,  d,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-s, w, -d,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		};

		float windmillVertices[] = {
			-0.2f,  -0.5f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
			0.2f,  -0.5f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f,
			-0.5f,  -0.2f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			0.5f,  -0.2f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			-0.1f,  -0.1f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,
			0.1f,  -0.1f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,

			-0.1f,  0.1f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
			0.1f,  0.1f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f,
			-0.5f,  0.2f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			0.5f,  0.2f,  0.1f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			-0.2f,  0.5f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,
			0.2f,  0.5f,  0.1f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,

			-0.2f,  -0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
			0.2f,  -0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f,
			-0.5f,  -0.2f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			0.5f,  -0.2f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			-0.1f,  -0.1f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,
			0.1f,  -0.1f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,

			-0.1f,  0.1f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
			0.1f,  0.1f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f,
			-0.5f,  0.2f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			0.5f,  0.2f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
			-0.2f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,
			0.2f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
		};
		GLuint windmillIndices[] = {
			 0,  1,  4,
			 4,  1,  5,
			 5,  3,  7,
			 7,  3,  9,
			 7, 10, 11,
			10,  7,  6,
			 6,  2,  8,
			 2,  6,  4,
			 5,  4,  6,
			 6,  5,  7,

			12, 13, 16,
			16, 13, 17,
			17, 15, 19,
			19, 15, 21,
			19, 22, 23,
			22, 19, 18,
			18, 14, 20,
			14, 18, 16,
			17, 16, 18,
			18, 17, 19,

		 	 3, 15,  9,
			 9, 15, 21,
			 9,  7, 19,
			19,  9, 21,
			 7, 19, 11,
			11, 19, 23,
			23, 22, 11,
			11, 22, 10,
			10, 22, 18,
			18, 10,  6,
		 	 6,  8, 20,
			20,  6, 18,
			 8,  2, 14,
			14, 20,  8,
			 2,  4, 14,
			14,  4, 16,
			16,  4, 12,
			12,  4,  0,
			 0,  1, 12,
			12,  1, 13,
			 1, 13,  5,
			 5, 13, 17,
			 5, 17,  3,
			 3, 17, 15,
		};
		// cylinderVert
			int num_segments = 144;
			int indeks = 0;
			float x = 0, y = 0, theta = 0, r = 0.5;
			int cylinderVervices_number = (num_segments + 1) * 8 * 2;
			int cylinderIndices_number = 3 * (2 * (num_segments - 1) + 2 * (num_segments + 1));
			float* cylinderVertices = new float[cylinderVervices_number];
			GLuint* cylinderIndices = new GLuint[cylinderIndices_number];
			srand(time(NULL));
			for (float k = 0; k < 2; k = k + 1) {
				x = 0; y = 0;
				for (int i = 0; i < num_segments + 1; i++) {
					cylinderVertices[indeks] = x; indeks++;
					cylinderVertices[indeks] = y; indeks++;
					theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
					x = r * cosf(theta);
					y = r * sinf(theta);
					cylinderVertices[indeks] = k / 5; indeks++;
					cylinderVertices[indeks] = 0.0f; indeks++;
					cylinderVertices[indeks] = 1.0f; indeks++;
					cylinderVertices[indeks] = 0.0f; indeks++;
					if (i == 0) {
						cylinderVertices[indeks] = 0.0; indeks++;
						cylinderVertices[indeks] = 1.0f; indeks++;
					}
					else {
						cylinderVertices[indeks] = (float)(rand()) / RAND_MAX; indeks++;
						cylinderVertices[indeks] = 0.0f; indeks++;
					}
				}
			}
			indeks = 0;
			int indeksI = 0;
			for (int k = 0; k < 2; k++) {
				for (int i = 0; i < num_segments - 1; i++) {
					cylinderIndices[indeksI] = GLuint(0 + (num_segments + 1)*k); indeks++; indeksI++;
					cylinderIndices[indeksI] = GLuint(indeks); indeks++; indeksI++;
					cylinderIndices[indeksI] = GLuint(indeks); indeks--; indeksI++;
				}
				indeks++;
				cylinderIndices[indeksI] = GLuint(indeks); indeks++;  indeksI++;
				cylinderIndices[indeksI] = GLuint(1 + (num_segments + 1)*k); indeksI++;
				cylinderIndices[indeksI] = GLuint(0 + (num_segments + 1)*k); indeksI++;
			}
			indeks = 1;
			for (int i = 0; i < num_segments - 1; i++) {
				cylinderIndices[indeksI] = indeks; indeksI++;
				cylinderIndices[indeksI] = indeks + 1; indeksI++;
				cylinderIndices[indeksI] = indeks + num_segments + 1; indeksI++;
				cylinderIndices[indeksI] = indeks + 1; indeksI++;
				cylinderIndices[indeksI] = indeks + num_segments + 1; indeksI++;
				cylinderIndices[indeksI] = indeks + num_segments + 2; indeksI++;
				indeks++;
			}
			cylinderIndices[indeksI] = num_segments; indeksI++;
			cylinderIndices[indeksI] = 2 * num_segments + 1; indeksI++;
			cylinderIndices[indeksI] = 1; indeksI++;
			cylinderIndices[indeksI] = num_segments + 2; indeksI++;
			cylinderIndices[indeksI] = 2 * num_segments + 1; indeksI++;
			cylinderIndices[indeksI] = 1; indeksI++;

		

		GLuint VBO, EBO, VAO, VAO2, VBO2, EBO2, VAO3, VBO3, EBO3, VAO4, VBO4, EBO4, VAO5, VBO5, EBO5, VAO6, VBO6, EBO6;
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO2);
		glGenBuffers(1, &VBO2);
		glGenBuffers(1, &EBO2);
		glGenVertexArrays(1, &VAO3);
		glGenBuffers(1, &VBO3);
		glGenBuffers(1, &EBO3);
		glGenVertexArrays(1, &VAO4);
		glGenBuffers(1, &VBO4);
		glGenBuffers(1, &EBO4);
		glGenVertexArrays(1, &VAO5);
		glGenBuffers(1, &VBO5);
		glGenBuffers(1, &EBO5);
		glGenVertexArrays(1, &VAO6);
		glGenBuffers(1, &VBO6);
		glGenBuffers(1, &EBO6);

		glEnable(GL_DEPTH_TEST);

		// domek

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(buldingVertices), buldingVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// polaczenia

		glBindVertexArray(VAO6);
		glBindBuffer(GL_ARRAY_BUFFER, VBO6);
		glBufferData(GL_ARRAY_BUFFER, sizeof(connectVertices), connectVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// dach 

		glBindVertexArray(VAO4);
		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(roofVertices), roofVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// trawa

		glBindVertexArray(VAO5);
		glBindBuffer(GL_ARRAY_BUFFER, VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// wiatrak
		glBindVertexArray(VAO2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(windmillVertices), windmillVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(windmillIndices), windmillIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//  ¿arno
		glBindVertexArray(VAO3);
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, 4* cylinderVervices_number, cylinderVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4* cylinderIndices_number, cylinderIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// prepare textures
		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "cegla.jpg");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "deski.jpg");
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "kamien.jpg");
		GLuint texture3 = LoadMipmapTexture(GL_TEXTURE3, "dachowka.jpg");
		GLuint texture4 = LoadMipmapTexture(GL_TEXTURE4, "trawa.jpg");


		float cameraSpeed = 0.005f; // adjust accordingly
		static GLfloat rot_angle = 0.0f;

		theProgramLight.Use();

		// main event loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			// Clear the colorbuffer
			glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			processInput(window);

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glm::mat4 trans;
			glm::mat4 trans2;
			glm::mat4 view;
			glm::mat4 projection;
			GLuint transformLoc;
			GLint viewLoc;
			GLint projLoc;

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			theProgramLight.Use();
			glUniform3fv(glGetUniformLocation(theProgramLight.get_programID(), "light.position"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(theProgramLight.get_programID(), "viewPos"), 1, &cameraPos[0]);
			glUniform3f(glGetUniformLocation(theProgramLight.get_programID(), "light.ambient"), ambient, ambient, ambient); // stale swiatlo
			glUniform3f(glGetUniformLocation(theProgramLight.get_programID(), "light.diffuse"), 0.6f, 0.6f, 0.6f); // rozproszenie
			glUniform3f(glGetUniformLocation(theProgramLight.get_programID(), "light.specular"), 0.1f, 0.1f, 0.1f); // odbicie swiatla // refleksja
			glUniform3f(glGetUniformLocation(theProgramLight.get_programID(), "material.specular"), 0.5f, 0.5f, 0.5f);
			glUniform1f(glGetUniformLocation(theProgramLight.get_programID(), "material.shininess"), 64.0f);
			transformLoc = glGetUniformLocation(theProgramLight.get_programID(), "transform");
			viewLoc = glGetUniformLocation(theProgramLight.get_programID(), "view");
			projLoc = glGetUniformLocation(theProgramLight.get_programID(), "projection");
			glm::mat4 model;
			glUniformMatrix4fv(glGetUniformLocation(theProgramLight.get_programID(), "model"), 1, GL_FALSE, &model[0][0]);

			// domek
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			//trans = glm::translate(trans, glm::vec3(0.0, -0.5, 0.0));
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 0);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 54);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// dach
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, texture3);
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 3);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO4);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// trawa, podloze 
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, texture4);
			//trans = glm::translate(trans, glm::vec3(0.0, 0.5, 0.0));
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 4);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO5);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			//trans = glm::translate(trans, glm::vec3(0.0, -0.5, 0.0));

			// polaczenia pion
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			trans = glm::translate(trans, glm::vec3(0.0, 0.0, 1.5));
			trans = glm::rotate(trans, glm::radians(rot_angle), glm::vec3(0, 1, 0));
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 1);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO6);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0, 1, 0));
			//trans = glm::translate(trans, glm::vec3(0.0, 0.0, -2.0));
			
			// polaczenia poziom
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			trans = glm::translate(trans, glm::vec3(0.0, 0.5, -0.5));
			trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1, 0, 0));
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0, 1 ,0));
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 1);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO6);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			trans = glm::rotate(trans, glm::radians(rot_angle), glm::vec3(0, 1, 0));
			trans = glm::rotate(trans, -glm::radians(90.0f), glm::vec3(1, 0, 0));
			trans = glm::translate(trans, glm::vec3(0.0, -0.5, 0.5));

			// zarno
			trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1, 0, 0));
			trans = glm::translate(trans, glm::vec3(0, 0, 0.3));
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0, 0, 1));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 2);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO3);
			glDrawElements(GL_TRIANGLES, 4 * 3 * (2 * (num_segments - 2) + 2 * num_segments), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			trans = glm::rotate(trans, glm::radians(rot_angle), glm::vec3(0, 0, 1));
			trans = glm::rotate(trans, -glm::radians(90.0f), glm::vec3(1, 0, 0));
			trans = glm::translate(trans, glm::vec3(0, 0, -0.3));
			trans = glm::translate(trans, glm::vec3(0.0, 0.0, -1.5));
			
			// wiatrak
			trans = glm::translate(trans, glm::vec3(0, 0.8, 0.81));
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0, 0, 1));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgramLight.get_programID(), "material.diffuse"), 1);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(VAO2);
			glDrawElements(GL_TRIANGLES, _countof(windmillIndices), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			trans = glm::rotate(trans, glm::radians(rot_angle), glm::vec3(0, 0, 1));
			trans = glm::translate(trans, glm::vec3(0, 0.8, -0.81));
			
			rot_angle += 0.05f;
			if (rot_angle >= 360.0f)
				rot_angle -= 360.0f;
			
			// Swap the screen buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO2);
		glDeleteBuffers(1, &VBO2);
		glDeleteBuffers(1, &EBO2);
		glDeleteVertexArrays(1, &VAO3);
		glDeleteBuffers(1, &VBO3);
		glDeleteBuffers(1, &EBO3);
		glDeleteVertexArrays(1, &VAO4);
		glDeleteBuffers(1, &VBO4);
		glDeleteBuffers(1, &EBO4);
		glDeleteVertexArrays(1, &VAO5);
		glDeleteBuffers(1, &VBO5);
		glDeleteBuffers(1, &EBO5);
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	glfwTerminate();

	return 0;
}
