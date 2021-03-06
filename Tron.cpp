// Tron.cpp : This game tests your ability to drive through an obstacle course with clunky controls!
//Controls:		A: Turn Left
//				D: Turn Right
//				1: Drive!
//Check your score in the command window after you crash! See if you can get the high score!

// stl include
#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <memory>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


// glfw includes
#include <GLFW/glfw3.h>


// include local files
#define GLM_ENABLE_EXPERIMENTAL
#include "Window.h" // the windows
#include "OpenGLDefaults.h" // some open gl and glew defaults
#include "VertexBuffers.h"  // create vertex buffer object
#include "ShaderProgram.h"  // create a shader program
#include "CommonTypes.h"  // types that all projects use
#include "Texture2D.h"

#include "RayIntersectionTest.h"
#include "ModelObj.h"

using namespace std;
using namespace cs557;

// Transformation pipeline variables
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;       // Store the view matrix
glm::mat4 rotated_view;

// The handle to the window object
GLFWwindow*         window;

// Define some of the global variables we're using for this sample

//Objects rendered
//Tron Grid
cs557::OBJModel grid;
Material grid_mat;

//Tron Bike
cs557::OBJModel bike;
Material bike_mat;

//Light Sources
LightSource light0;
LightSource light1;

//= new GLObjectObj("C:/Users/jaceh/Desktop/ME557/Final Project/Models/TRON.obj");
vector<glm::vec3> vertices_grid;
vector<glm::vec3> vertices_bike;

// to transform the bike
glm::mat4 g_tranform_bike;
glm::mat4 g_tranform_bike_result;
glm::mat4 g_view_result;

unsigned int texture_id_0;
unsigned int texture_id_1;
unsigned int texture_id_2;
unsigned int texture_id_3;

int score;

int starter = 0;
float g_delta = 1.0; //Speed of bike

// This is the callback we'll be registering with GLFW for keyboard handling.
// The only thing we're doing here is setting up the window to close when we press ESC
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	bool move = true;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == 49 && action == GLFW_PRESS) // 1
	{
		starter = 1;
	}
	
	else if (key == 50 && action == GLFW_PRESS) // 2
	{
		glm::mat4 camera_transformation = glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}


	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////
	// Rotation
	if ((key == 65 && action == GLFW_REPEAT) || (key == 65 && action == GLFW_PRESS)) // key a
	{
		//cout <<  "key a pressed" << endl;
		g_tranform_bike = g_tranform_bike * glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		bike_mat.model_matrix = g_tranform_bike;
	}

	else if ((key == 68 && action == GLFW_REPEAT) || (key == 68 && action == GLFW_PRESS)) // key d
	{
		//cout <<  "key d pressed" << endl;
		g_tranform_bike = g_tranform_bike * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		bike_mat.model_matrix = g_tranform_bike;
	}

	//cout << key;
}

void CreateLights(void) {

	//Create a first light
	light0.index = 0;
	light0.pos = glm::vec3(0.0f, 20.0f, 0.0f);
	light0.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	light0.color = glm::vec3(1.0f, 1.0f, 1.0f);
	light0.intensity = 0.5;

	light1.index = 1;
	light1.pos = glm::vec3(0.0f, 20.0f, 0.0f);
	light1.dir = glm::vec3(0.0f, -1.0f, 0.0f);
	light1.color = glm::vec3(1.0f, 1.0f, 1.0f);

	light1.cutoff_in = 0.2;
	light1.cutoff_out = 0.2;
	light1.intensity = 1.0;
	light1.type = SPOT;
}

void CreateGridModel(void) {

	//Define the grid material parameters
	grid_mat.diffuse_mat = glm::vec3(0.5, 0.5, 0.5);
	grid_mat.diffuse_int = 0.25;

	grid_mat.specular_mat = glm::vec3(1.0, 1.0, 1.0);
	grid_mat.specular_int = 0.4;
	grid_mat.specular_s = 1.0;

	grid_mat.ambient_mat = glm::vec3(0.2, 0.2, 0.2);
	grid_mat.ambient_int = 1.0;

	int program = cs557::LoadAndCreateShaderProgram("lit_scene.vs", "lit_scene.fs");

	vertices_grid = grid.create("C:/Users/jaceh/Desktop/ME557/Final Project/Models/map.obj", program);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Textures
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int blend_mode = 0; //0: Use textures | 1: Do not use textures

	//Loading the texture
	LoadAndCreateTexture2D("C:/Users/jaceh/Desktop/ME557/Final Project/grid.bmp", &texture_id_0, GL_REPEAT, GL_TEXTURE0);
	LoadAndCreateTexture2D("C:/Users/jaceh/Desktop/ME557/HW#4/gradient.bmp", &texture_id_1, GL_REPEAT, GL_TEXTURE1);
	

	
	// Activate the texture unit and bind the texture.
	glUseProgram(program);

	//Texture 1: Grid Pattern
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id_0);

	// Fetch the texture location and set the parameter to 0.
	// Note that 0 is the number of the texture unit GL_TEXTURE0.
	int texture_location = glGetUniformLocation(program, "texture0");
	glUniform1i(texture_location, 0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Texture 2: Different Pattern
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_id_1);

	// Fetch the texture location and set the parameter to 0.
	// Note that 0 is the number of the texture unit GL_TEXTURE0.
	int pattern_location = glGetUniformLocation(program, "texture1");
	glUniform1i(pattern_location, 1);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUniform1i(glGetUniformLocation(program, "blend_mode"), blend_mode);

	grid_mat.setAllUniform(grid.getProgram());
	light0.setAllUniform(grid.getProgram());
	light1.setAllUniform(grid.getProgram());


	glUseProgram(0);
}

void CreateBikeModel(void) {
	
	//Material Properties
	bike_mat.diffuse_mat = glm::vec3(0.0, 0.0, 0.0);
	bike_mat.diffuse_int = 0.3;
	bike_mat.specular_mat = glm::vec3(1.0, 0.0, 0.5);
	bike_mat.specular_int = 0.5;
	bike_mat.specular_s = 0.25;
	bike_mat.ambient_mat = glm::vec3(1.0, 1.0, 1.0);
	bike_mat.ambient_int = 0.5;

	int program = cs557::LoadAndCreateShaderProgram("lit_scene.vs", "lit_scene.fs"); //Load and create shader program

	vertices_bike = bike.create("C:/Users/jaceh/Desktop/ME557/Final Project/Models/TRON.obj",program); //Load object (returns vertices used for RayIntersectionTest)
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Textures
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	int blend_mode = 0; //0: Use textures | 1: Do not use textures

	//Loading the texture
	LoadAndCreateTexture2D("C:/Users/jaceh/Desktop/ME557/HW#4/gradient.bmp", &texture_id_2, GL_REPEAT, GL_TEXTURE2);
	LoadAndCreateTexture2D("C:/Users/jaceh/Desktop/ME557/HW#4/gradient.bmp", &texture_id_3, GL_REPEAT, GL_TEXTURE3);

	// Activate the texture unit and bind the texture.
	glUseProgram(program);

	//Texture 1: Grid Pattern
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_id_2);

	// Fetch the texture location and set the parameter to 0.
	// Note that 0 is the number of the texture unit GL_TEXTURE0.
	int texture_location = glGetUniformLocation(program, "texture2");
	glUniform1i(texture_location, 2);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Texture 2: Different Pattern
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture_id_3);

	// Fetch the texture location and set the parameter to 0.
	// Note that 0 is the number of the texture unit GL_TEXTURE0.
	int pattern_location = glGetUniformLocation(program, "texture3");
	glUniform1i(pattern_location, 3);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glUniform1i(glGetUniformLocation(program, "blend_mode"), blend_mode);
	

	bike_mat.setAllUniform(bike.getProgram());
	light0.setAllUniform(bike.getProgram());
	light1.setAllUniform(bike.getProgram());

	glUseProgram(0);
}

void CreateScene(void) {

	projectionMatrix = glm::perspective(1.0f, (float)800/(float)600, 0.1f, 10000.0f);
	viewMatrix = glm::lookAt(glm::vec3(-10.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 camera_delta = glm::rotate(-0.2f, glm::vec3(1.0f, 0.0f, 0.0f));

	viewMatrix = camera_delta * viewMatrix * glm::inverse(bike_mat.model_matrix);
	
	//viewMatrix = glm::lookAt(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	CreateLights();
	CreateBikeModel();
	CreateGridModel();
}

void RayTest(void) {
	
	glm::vec3 s(g_tranform_bike[3][0], g_tranform_bike[3][1], g_tranform_bike[3][2]);
	glm::vec3 e(g_tranform_bike[3][0], g_tranform_bike[3][1]-1, g_tranform_bike[3][2]);
	glm::vec3 x(g_tranform_bike[3][0]+1, g_tranform_bike[3][1], g_tranform_bike[3][2]);
	glm::vec3 xx(g_tranform_bike[3][0], g_tranform_bike[3][1], g_tranform_bike[3][2]+1);

	vector<glm::vec3> res;

	// perform the ray intersectiont test.

	RayIntersectionTest::intersect(s, e, vertices_grid, grid_mat, res);//With grid

	if (res.size() > 0) {
		glm::vec3 position = res[0];

		g_tranform_bike_result = g_tranform_bike;
		g_tranform_bike_result[3][0] = position[0];
		g_tranform_bike_result[3][1] = position[1];
		g_tranform_bike_result[3][2] = position[2];
		bike_mat.model_matrix = g_tranform_bike_result;
	}

	bool crash0 = RayIntersectionTest::intersect(s, x, vertices_grid, grid_mat, res);//With trail
	bool crash1 = RayIntersectionTest::intersect(s, xx, vertices_grid, grid_mat, res);//With grid
	if (crash0 == true || crash1 == true) {
		cout << "You crashed!" << " Your score was: " << score << "! Play again!";
		throw std::exception();
	}
	// pick the first result if one is available
	if (res.size() > 0) {
		glm::vec3 position = res[0];

		g_tranform_bike_result = g_tranform_bike;
		g_tranform_bike_result[3][0] = position[0];
		g_tranform_bike_result[3][1] = position[1];
		g_tranform_bike_result[3][2] = position[2];
		bike_mat.model_matrix = g_tranform_bike_result;
	}
}

void Init(void) {
	// Init the GLFW Window
	window = cs557::initWindow();

	// Init the glew api
	cs557::initGlew();

	CreateScene();
}

void Draw(void) {
	
	glm::mat4 object_transform = glm::mat4();
	glm::mat4 camera_matrix = glm::mat4();
	glm::mat4 camera_delta = glm::rotate(-0.2f, glm::vec3(1.0f, 0.0f, 0.0f));

	// Set up our green background color
	static const GLfloat clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Enable depth test
	// ignore this line, it allows us to keep the distance value after we proejct each object to a 2d canvas.
	glEnable(GL_DEPTH_TEST);

	// Set the keyboard callback so that when we press ESC, it knows what to do.
	glfwSetKeyCallback(window, keyboard_callback);

	// This is our render loop. As long as our window remains open (ESC is not pressed), we'll continue to render things.
	while (!glfwWindowShouldClose(window))
	{
		
		// Clear the entire buffer with our green color (sets the background to be green).
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, clear_depth);


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// This renders the objects

		//Translation for Bike
		if (starter == 1)
		{
			g_tranform_bike = g_tranform_bike * glm::translate(glm::vec3(g_delta, 0.0f, 0.0f));
			bike_mat.model_matrix = g_tranform_bike;
		}
		
		RayTest();
		score = score + 1;
		//viewMatrix = glm::lookAt(glm::vec3(0.0f, 50.0f, -405.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//Draws bike and grid
		bike.draw(projectionMatrix, viewMatrix, bike_mat.model_matrix);
		grid.draw(projectionMatrix, viewMatrix, grid_mat.model_matrix);

		//Navigation camera
		glm::mat4 camera_delta = glm::rotate(-0.2f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 camera_transformation = glm::lookAt(glm::vec3(-10.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		object_transform = bike_mat.model_matrix;
		viewMatrix = camera_delta * camera_transformation * glm::inverse(object_transform);

		glm::lookAt(vec3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]),
					vec3(viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2]),
					vec3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]));
		
		glUseProgram(0);

		// Swap the buffers so that what we drew will appear on the screen.
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

}

int main(int argc, const char * argv[])
{
	Init();
	Draw();
}

