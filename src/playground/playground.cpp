#include "playground.h"
#include "gameUtils.h"
#include "model.h"
#include "modelLoader.h"
#include "gameData.h"
#include "loadSTL.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>
#include "parse_stl.h"

GLFWwindow* window;

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#include <chrono>
#include <thread>
using namespace std::chrono;

#include <fstream>
#include <string.h>
#include <string>


int main(void)
{

	srand(time(0));

	bool windowInitialized = initializeWindow();
	if (!windowInitialized) return -1;

	//Initialize vertex buffer
	bool vertexbufferInitialized = initializeVertexbuffer();
	if (!vertexbufferInitialized) return -1;

	//Initialize model manager
	spaceShip = make_shared<SpaceShip>();
	world = std::make_shared<World>(WORLD_SIZE, WORLD_OFFSET);
	worldManager = std::make_shared<WorldManager>(world);

	world->addObject(spaceShip);

	shared_ptr<ModelBase> bunny{ loadIndexedSTLModel("bunny-decimated.stl") };
	shared_ptr<ModelInstance> bunnyInstance{ make_shared<ModelInstance>(bunny) };

	bunnyInstance->getModelTransformation()->scale(0.01, 0.01, 0.01);
	bunnyInstance->getModelTransformation()->translate(5, 0, 0);

	//modelManager->add(bunnyInstance);

	shared_ptr<ModelBase> stone{ loadModel(loadOBJ("./models/asteroids/asteroid1/asteroid.obj",true)) };

	worldManager->addAsteroidModel(stone);
	//worldManager->addAsteroidModel(bunny);

	// staeroids from https://opengameart.org/content/low-poly-rocks
	//shared_ptr<BMP> polyTexture{ loadBMP("rock_weathered_10.bmp") };

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	initializeVPTransformation();

	high_resolution_clock::time_point tStartLoop = high_resolution_clock::now();

	//start animation loop until escape key is pressed
	do {
		if (glfwGetKey(window, GLFW_KEY_A)) {
			spaceShip->moveZ(-1);
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			spaceShip->moveZ(1);
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			spaceShip->moveY(-1);
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			spaceShip->moveY(1);
		}

		auto now = high_resolution_clock::now();
		long elapsed = duration_cast<milliseconds>(now - tStartLoop).count();

		long dt = elapsed;
		if (dt > 50) dt = 50;

		world->updateAM(dt);
		worldManager->update(spaceShip->getLocation(), spaceShip->getCamPos());

		updateAnimationLoop();

		if (elapsed < 20)
			std::this_thread::sleep_for(milliseconds(20 - elapsed));
		tStartLoop = now;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	//Cleanup and close window
	cleanupVertexbuffer();
	glDeleteProgram(programID);
	closeWindow();

	return 0;
}

void updateAnimationLoop()
{


	MMatrixID = glGetUniformLocation(programID, "M");
	VMatrixID = glGetUniformLocation(programID, "V");
	GLuint mInvMatrixId = glGetUniformLocation(programID, "Minv");
	//TODO container for this stuff
	GLuint viewDirectionId = glGetUniformLocation(programID, "viewDirection");
	GLuint viewPosId = glGetUniformLocation(programID, "viewPosIn");
	GLuint lightPosId = glGetUniformLocation(programID, "lightPosIn");
	GLuint hasTextureId = glGetUniformLocation(programID, "hasTextureIn");

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	initializeVPTransformation();

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	//glUniformMatrix4fv(VPMatrixID, 1, GL_FALSE, &VP[0][0]);

	/*
	if (glfwGetKey(window, GLFW_KEY_W)) curr_y += 0.01;
	else if (glfwGetKey(window, GLFW_KEY_S)) curr_y -= 0.01;
	else if (glfwGetKey(window, GLFW_KEY_A)) curr_x -= 0.01;
	else if (glfwGetKey(window, GLFW_KEY_D)) curr_x += 0.01;
	else if (glfwGetKey(window, GLFW_KEY_E)) curr_angle += 0.05;
	else if (glfwGetKey(window, GLFW_KEY_Q)) curr_angle -= 0.05;
	glm::mat4 transformation;//additional transformation for the model
	transformation[0][0] = 1.0; transformation[1][0] = 0.0; transformation[2][0] = 0.0; transformation[3][0] = curr_x;
	transformation[0][1] = 0.0; transformation[1][1] = 1.0; transformation[2][1] = 0.0; transformation[3][1] = curr_y;
	transformation[0][2] = 0.0; transformation[1][2] = 0.0; transformation[2][2] = 1.0; transformation[3][2] = 0.0;
	transformation[0][3] = 0.0; transformation[1][3] = 0.0; transformation[2][3] = 0.0; transformation[3][3] = 1.0;*/

	//MV = MV * transformation;

	glUniformMatrix4fv(VPMatrixID, 1, GL_FALSE, &VP[0][0]);
	glUniformMatrix4fv(VMatrixID, 1, GL_FALSE, &V[0][0]);

	vec4 viewDirection = spaceShip->getViewDirection();
	vec3 viewPos = spaceShip->getCamPos();
	vec3 lightPos = spaceShip->getPos() + vec3(12, 1, 5);

	glUniform4fv(viewDirectionId, 1, &viewDirection[0]);
	glUniform3fv(viewPosId, 1, &viewPos[0]);
	glUniform3fv(lightPosId, 1, &lightPos[0]);

	world->updateMM(programID, MMatrixID, hasTextureId, mInvMatrixId);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool initializeWindow()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth = 1024, windowHeight = 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, windowWidth = width, windowHeight = height);
		});

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.0f);

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);

	return true;
}

bool initializeVPTransformation()
{
	// Get a handle for our "MVP" uniform
	GLuint MatrixIDnew = glGetUniformLocation(programID, "VP");
	VPMatrixID = MatrixIDnew;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 1000.0f);
	//glm::mat4 Projection = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	// Camera matrix
	float distance = 8;
	glm::mat4 View = spaceShip->getView();
	/*glm::mat4 View = glm::lookAt(
		spaceShip.cam, // Camera is at (4,3,-3), in World Space
		cam + viewDirection, // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);*/
	// Model matrix : an identity matrix (model will be at the origin)
	//glm::mat4 Model = glm::mat4(1.0f);

	//Model = glm::rotate(Model, curr_angle, glm::vec3(0.0f, 1.0f, 1.0f));


	// Our ModelViewProjection : multiplication of our 3 matrices
	VP = Projection * View /* Model*/; // Remember, matrix multiplication is the other way around

	V = View;

	return true;

}

bool initializeVertexbuffer()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	vertexbuffer_size = 1;
	static const GLfloat g_vertex_buffer_data[] = {
	  -1.0f,-1.0f,-1.0f, // triangle 1 : begin
	  -1.0f,-1.0f, 1.0f,
	  -1.0f, 1.0f, 1.0f, // triangle 1 : end
	  -3.0f,-3.0f,-1.0f, // triangle 2 : begin
	  -3.0f,-3.0f, 1.0f,
	  -3.0f,-1.0f, 1.0f, // triangle 2 : end
	};

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return true;
}

bool cleanupVertexbuffer()
{
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	return true;
}

bool closeWindow()
{
	glfwTerminate();
	return true;
}
