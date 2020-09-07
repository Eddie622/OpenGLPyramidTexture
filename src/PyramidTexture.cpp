/*
 * PyramidTexture.cpp
 *
 *  Created on: Aug 2, 2020
 *      Author: Admin
 */

/*Header Inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM Math Header Inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image Loader Inclusion
#include "../SOIL2/SOIL2.h"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Pyramid Texture" // Window title Macro

/* Shader program Macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, texture;
GLfloat degrees = glm::radians(-45.0f); // Converts float to degrees

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);

/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;
	layout (location = 2) in vec2 textureCoordinate;

	out vec2 mobileTextureCoordinate;

	// Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
		mobileTextureCoordinate = vec2(textureCoordinate.x, 1.0f - textureCoordinate.y); // flips the texture horizontally
	}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,

	in vec2 mobileTextureCoordinate;

	out vec4 gpuTexture; // Variable to pass color data to the GPU

	uniform sampler2D uTexture; // Useful when working with multiple textures

	void main(){

		gpuTexture = texture(uTexture, mobileTextureCoordinate); // Sends texture to the GPU for rendering

	}
);

/* Main Program */
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std ::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

	UCreateShader();
	UCreateBuffers();
	UGenerateTexture();

	glUseProgram(shaderProgram); // Uses the Shader program

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Sets background color to black

	glutDisplayFunc(URenderGraphics);

	glutMainLoop();

	// Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;

}

/* Resizes window */
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

void URenderGraphics(void)
{
	glEnable(GL_DEPTH_TEST); // Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glBindVertexArray(VAO); // Active the Vertex Array object before rendering and transforming them

	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f)); // Place the object at the center of the viewport
	model = glm::rotate(model, degrees, glm::vec3(0.0, 1.0f, 0.0f)); // Rotate the object y -45 degrees
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increase the object size by a scale of 2

	// Transforms the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); // Moves the world 0.5 units on X and -5 units in Z

	// Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Retrieves and passes transform matrices to the Shader Program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	glBindTexture(GL_TEXTURE_2D, texture); // Activate the texture

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0); // Deactivate the Vertex Array Object

	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame

}

/* Creates the Shader program */
void UCreateShader()
{
	//Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attaches the Vertex shader to the source code
	glCompileShader(vertexShader); // Compiles the Vertex shader

	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //Attaches the Fragment shader to the source code
	glCompileShader(fragmentShader); // Compiles the Fragment shader

	//Shader program
	shaderProgram = glCreateProgram(); //Create the Shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); //Attach Vertex shader to the Shader program
	glAttachShader(shaderProgram, fragmentShader);; // Attach Fragment shader to the shader program
	glLinkProgram(shaderProgram); // Link vertex and fragment shader to shader program

	//Delete the Vertex and Fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UCreateBuffers()
{

	GLfloat vertices[] = {

					// Positions			//Texture Coordinates
					// Front Face
					 0.0f,  0.5f,  0.0f,	0.0f, 0.5f,
					-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,

					 // Left Face
					 0.0f,  0.5f,  0.0f,	0.0f, 0.5f,
					-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
					-0.5f, -0.5f,  0.5f,	1.0f, 0.0f,

					// Back Face
					 0.0f,  0.5f,  0.0f,	0.0f, 0.5f,
					-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,

					// Right Face
					 0.0f,  0.5f,  0.0f,	0.0f, 0.5f,
					 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,

					// Bottom Left
					-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,

					// Bottom Right
					-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,	1.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,	1.0f, 1.0f

	};

	//Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Active the vertex Array object before binding and setting any VBOs and vertex Attribute Pointers
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copy vertices to VBO

	//Set attribute pointer 0 to hold Position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enable vertex attribute

	//Set attribute pointer 2 to hold Texture coordinate data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // Enable vertex attribute

	glBindVertexArray(0); // Deactivates the VAO which is good practice
}

void UGenerateTexture(){

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;

	unsigned char* image = SOIL_load_image("snhu.jpg", &width, &height, 0, SOIL_LOAD_RGB); // Loads texture file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}



