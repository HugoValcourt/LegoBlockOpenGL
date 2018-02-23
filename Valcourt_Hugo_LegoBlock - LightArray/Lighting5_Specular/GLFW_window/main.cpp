#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> //for matrix transformation functions

#include "glslprogram.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "Config.h"
#include "Joystick.h"
#include "Cube.h"
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>

//this macro helps calculate offsets for VBO stuff
//Pass i as the number of bytes for the offset, so be sure to use sizeof() 
//to help calculate bytes accurately.
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Mesh *mesh = NULL;

Cube *cube = NULL;
std::vector<Cube*> cubeList;
GLFWwindow* window = NULL;

Configuration configuration;

Joystick joystick;

glm::vec3 cameraPosition;
float cameraPitch = 0, cameraYaw = 0;
glm::vec4 cameraDir;
float scalingMovement = 6.f;
float scalingRotation = 2.f;


//lighting
struct Light
{
	glm::vec3 position;
	glm::vec3 colour;
	float power;
};

//lights
std::vector<Light> lights;
Light light1;

void MakeLight()
{
	//light #1
	light1.position = glm::vec3(-5, -5, 0);
	light1.colour = glm::vec3(0.8f, 0.9f, 0.7f);
	light1.power = 10.f;
	lights.push_back(light1);

	//light #2
	light1.position = glm::vec3(5, 10, 5);
	light1.colour = glm::vec3(1.0f, 0.7f, 0.7f);
	light1.power = 50.f;
	lights.push_back(light1);
}

void UpdateLight(GLSLProgram *shaders)
{
	//lighting
	shaders->setUniform("numLights", (int)lights.size());
	for (int i = 0; i < lights.size(); ++i)
	{
		std::stringstream ss;
		ss << "lightArray[" << i << "].";
		std:string lightString;

		//light position	
		lightString = ss.str() + "position";
		shaders->setUniform(lightString.c_str(), lights[i].position);
		//light colour
		lightString = ss.str() + "colour";
		shaders->setUniform(lightString.c_str(), lights[i].colour);
		//light power
		lightString = ss.str() + "power";
		shaders->setUniform(lightString.c_str(), lights[i].power);
	}
}

void MakeMarioLego(GLSLProgram *shaders, float x, float y, float z)
{
	//Feet
	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(0.8f, 0.5f, 0.19f, 1.f));
	cube->pos = glm::vec3(x + (-4), y + 0, z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(0.8f, 0.5f, 0.19f, 1.f));
	cube->pos = glm::vec3(x + (4), y + 0, z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(0.8f, 0.5f, 0.19f, 1.f));
	cube->pos = glm::vec3(x + (3.5), y + 1.25, z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(0.8f, 0.5f, 0.19f, 1.f));
	cube->pos = glm::vec3(x + (-3.5), y + 1.25, z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
	//Feet END

	//Legs
	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x -2.5, y + (1.25 * 2), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (2.5), y + (1.25 * 2), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 8, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 3), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
	//Legs END

	//Torso
	cube = new Cube(shaders, 6, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 4), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

		//His Left hand
	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (4.5), y + (1.25 * 4), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
	
	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (5), y + (1.25 * 3), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (3.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

		//His Right hand
	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-4.5), y + (1.25 * 4), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-5), y + (1.25 * 3), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (-3.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

		//His Left Arm
	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (4), y + (1.25 * 6), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (3.5), y + (1.25 * 7), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

		//His Right Arm
	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (-4), y + (1.25 * 6), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (-3.5), y + (1.25 * 7), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (-3), y + (1.25 * 8), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

		//Gold buttons
	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.0f, 1.0f, 0.2f, 1.f));
	cube->pos = glm::vec3(x + (1.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.0f, 1.0f, 0.2f, 1.f));
	cube->pos = glm::vec3(x + (-1.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (2.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-2.5), y + (1.25 * 5), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 6), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	for (int i = 0; i < 2; i++)
	{
		cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
		cube->pos = glm::vec3(x + (-1.5), y + (1.25 * (i + 7)), z);
		cube->SetAxis(glm::vec3(0, 1, 0));
		cubeList.push_back(cube);
	}

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (1.5), y + (1.25 * 7), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 7), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (0.5), y + (1.25 * 8), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
	//Torso END

	//Head
	cube = new Cube(shaders, 7, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0.5), y + (1.25 * 9), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-1), y + (1.25 * 10), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		
	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-4), y + (1.25 * 10), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

		//Stach
	cube = new Cube(shaders, 4, 1.25, 2, glm::vec4(0.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (3), y + (1.25 * 10), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (2.5), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
		//

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (4.5), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (3), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0), y + (1.25 * 13), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (1.5), y + (1.25 * 13), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (2.5), y + (1.25 * 13), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (1.5), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (0.5), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 2, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-2), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-3.5), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-4.5), y + (1.25 * 11), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-4.5), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-3.5), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(1.f, 1.f, 1.f, 1.f));
	cube->pos = glm::vec3(x + (-0.5), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 1, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-2.5), y + (1.25 * 12), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 3, 1.25, 2, glm::vec4(0.7f, 0.5f, 0.3f, 1.f));
	cube->pos = glm::vec3(x + (-2.5), y + (1.25 * 13), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	//Hat
	cube = new Cube(shaders, 9, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + 0.5, y + (1.25 * 14), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(shaders, 5, 1.25, 2, glm::vec4(1.f, 0.f, 0.f, 1.f));
	cube->pos = glm::vec3(x + (-0.5), y + (1.25 * 15), z);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);
	//

	//Head END

}

// from https://www.3dgep.com/understanding-the-view-matrix/
// Pitch should be in the range of [-90 ... 90] degrees and yaw
// should be in the range of [0 ... 360] degrees.
glm::mat4 FPSViewRH(glm::vec3 eye, float pitch, float yaw)
{
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	glm::vec3 xaxis = glm::vec3(cosYaw, 0, -sinYaw);
	glm::vec3 yaxis = glm::vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	glm::vec3 zaxis = glm::vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	glm::mat4 viewMatrix = glm::mat4(
		glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		glm::vec4(-glm::dot(xaxis, eye), -glm::dot(yaxis, eye), -glm::dot(zaxis, eye), 1)
	);

	return viewMatrix;
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
	if (!LoadConfig(configuration))
	{
		std::cout << "ERROR reading the config file!\n\n";
	}
	else
		DisplayConfig(configuration);

	glfwSetErrorCallback(error_callback);

	if(!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(800, 600, "Context example", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);// now we have an OpenGL context for this thread.

	//use GLEW to initialiaze modern opengl functionality supported by the GPU drivers
	glewInit();

	//joystick stuff
	joystick.Init();

	//Make Texture Manager
	TextureManager texManager;

	//load shaders
	GLSLProgram *shaders;
	shaders = new GLSLProgram();

	//Builds a pixal art 8 bit Mario with legos
	//int i = 0;
	//int j = 0;
	//for (i = 0; i < 20; i++)
	//{
	//	for(j = 0; j < 20; j++)
		MakeMarioLego(shaders, 0, 0, -10);
	//}

	shaders->compileShaderFromFile("vertex.glsl", GLSLShader::GLSLShaderType::VERTEX);
	shaders->compileShaderFromFile("fragment.glsl", GLSLShader::GLSLShaderType::FRAGMENT);

	//bind attributes for the shader layout BEFORE linking the shaders!
	//We only need to do this if we can't use the layout command from within the shader code.
	shaders->bindAttribLocation(0, "position");
	shaders->bindAttribLocation(1, "normal");
	shaders->bindAttribLocation(2, "texUV");

	shaders->link();
	shaders->use();

	//MESH LOAD
	//mesh = new Mesh(&texManager, &shaders, "skull.s3d");

	glfwSetKeyCallback(window, key_callback);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//turn on single-sided polygons
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//Primitive restart
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFF);//0xFF maximum short value

	//view matrix to transform the camera
	glm::mat4 viewMatrix;
	//projection matrix to project the scene onto the monitor
	glm::mat4 projectionMatrix;

	//...and an accumulator for rotatation:
	float angle = 0.f;

	//timer vars
	double currentTime = glfwGetTime();
	double previousTime = currentTime;
	double timePassed;

	cameraPosition = glm::vec3(0, 0, 0);

	//Make lights
	MakeLight();

	while(!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		timePassed = currentTime - previousTime;
		if (timePassed > 0) previousTime = currentTime;

		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//update projection (needed if window size can be modified)
		projectionMatrix = glm::perspective(45.0f, ratio, 0.1f, 10000.f);

		//send the matrix to the shader
		shaders->setUniform("projectionMatrix", projectionMatrix);

		joystick.DoJoystick(); //handle joystick polling
							   //process joystick values
		joystick.ProcessJoystickAxis(joystick.joystickPositionAxis1, configuration);
		joystick.ProcessJoystickAxis(joystick.joystickPositionAxis2, configuration);
		joystick.ProcessJoystickAxis(joystick.joystickPositionAxis4, configuration);
		joystick.ProcessJoystickAxis(joystick.joystickPositionAxis3, configuration);


		cameraYaw += joystick.joystickPositionAxis3 * scalingRotation * timePassed * configuration.axis_yaw;
		cameraPitch += joystick.joystickPositionAxis4 * scalingRotation * timePassed * configuration.axis_pitch;

		//move ahead
		cameraDir = glm::vec4(0, 0, -1, 0);
		viewMatrix = glm::mat4(1.f);
		viewMatrix = glm::rotate(viewMatrix, cameraYaw, glm::vec3(0, 1.f, 0));
		viewMatrix = glm::rotate(viewMatrix, cameraPitch, glm::vec3(1.f, 0, 0));
		cameraDir = viewMatrix * cameraDir;

		cameraPosition += glm::vec3(cameraDir) * scalingMovement * joystick.joystickPositionAxis2 * (float)timePassed * configuration.axis_move;

		//strafe
		cameraDir = glm::vec4(1, 0, 0, 0);
		viewMatrix = glm::mat4(1.f);
		//viewMatrix = glm::rotate(viewMatrix, cameraPitch, glm::vec3(1.f, 0, 0));
		viewMatrix = glm::rotate(viewMatrix, cameraYaw, glm::vec3(0, 1.f, 0));
		cameraDir = viewMatrix * cameraDir;

		cameraPosition += glm::vec3(cameraDir) * scalingMovement * joystick.joystickPositionAxis1 * (float)timePassed * configuration.axis_strafe;

		//update camera position via the view matrix
		viewMatrix = FPSViewRH(cameraPosition, cameraPitch, cameraYaw);

		//send the matrix to the shader
		shaders->setUniform("viewMatrix", viewMatrix);

		//update angle
		angle += timePassed * 1;

		//Update litghs
		UpdateLight(shaders);

		//material shinyness
		shaders->setUniform("shinyness", 1000.5f);
		//material specular "power" : increasing this focuses the specular reflection size
		shaders->setUniform("specularPower", 90.f);
		
		//Update and draw elements from the Cube vector
		for (int i = 0; i < cubeList.size(); i++)
		{
			cubeList[i]->Update(timePassed);
			cubeList[i]->Draw();
		}

		glfwSwapBuffers(window); //display the graphics buffer to the screen
		glfwPollEvents(); //prime the message pump that GLFW uses for input events
	}

	//Delete the cubes and clear the vector
	for (auto cube : cubeList) delete cube;
	cubeList.clear();

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}