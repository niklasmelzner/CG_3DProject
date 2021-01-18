#include "playground.h"
#include "gameUtils.h"
#include "engine/loadSTL.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>

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
	// seeding random
	srand(time(0));

	// create world
	world = std::make_shared<World>(WORLD_SIZE, WORLD_OFFSET);

	// create and intialize the Renderer
	renderer = make_shared<Renderer>("Asteroids 3D", 1024, 700);
	renderer->init();
	renderer->setWorld(world);

	// load models in "gameUtils.h"
	initModels();

	// create SpaceShip
	spaceShip = make_shared<SpaceShip>();

	// place SpaceShip in world
	world->addObject(spaceShip);

	// create the WorldManager (asteroid management)
	worldManager = make_shared<WorldManager>(world);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	// initialize program, projection of the Renderer
	renderer->setProgram(programID);
	renderer->setProjection(radians(60.0f), 0.1f, 1000.0f);

	// add Asteroid-ModelBases to WorldManager
	worldManager->addAsteroidModel(loadIndexedSTLModel("bunny-decimated.stl"), 1);
	worldManager->addAsteroidModel(loadModel(loadOBJ("./models/asteroids/asteroid1/asteroid.obj", true)), 50);
	worldManager->addAsteroidModel(loadModel(loadOBJ("./models/asteroids/asteroid2/asteroid.obj", true)), 15);
	worldManager->addAsteroidModel(loadModel(loadOBJ("./models/asteroids/asteroid3/asteroid.obj", true)), 30);

	// connect the View dynamically to the SpaceShip's View
	renderer->setViewSupplier([=]() {return spaceShip->getView(); });

	// create an AttrIds-container (which contains all names of attributes used in shaders)
	shared_ptr<AttrIds> attrIds{ make_shared<AttrIds>(programID, "vertexPosition","normalIn","texCoordIn",
		"VP", "V", "M", "Minv","viewPosIn", "hasTexture","effectCount","lightCount") };
	attrIds->initEffectIds("effects", "type", "value", 5);
	attrIds->initLightIds("lights", "type", "pos", "color", "modifiers", "modifiers1", 32);

	// apply the attributeIds to the Renderer
	renderer->setShaderAttrIds(attrIds);

	// get some attribute-locations from the vertex shader and add an "ModelManager"-update-listener to the Render to update their values
	GLuint viewPosId = glGetUniformLocation(programID, "viewPosIn");

	renderer->setViewPosSupplier([=]() {return spaceShip->getCamPos(); });

	// main updateListener (called before updating)
	renderer->onUpdate([=](shared_ptr<Renderer> renderer, long dt) {
		// listening on keys and handing them over to the SpaceShip
		if (glfwGetKey(renderer->window, GLFW_KEY_A)) spaceShip->moveZ(-1);
		if (glfwGetKey(renderer->window, GLFW_KEY_D)) spaceShip->moveZ(1);
		if (glfwGetKey(renderer->window, GLFW_KEY_W)) spaceShip->moveY(-1);
		if (glfwGetKey(renderer->window, GLFW_KEY_S)) spaceShip->moveY(1);
		if (glfwGetKey(renderer->window, GLFW_KEY_SPACE)) {// shot projectile
			if (!spacePressed) {
				spacePressed = true;
				// get SpaceShip-transformation
				mat4 mat{ spaceShip->getModel()->getTransformInstance() };
				// get current direction of SpaceShip
				vec3 direction{ normalize(vec3(mat * vec4(1,0,0,0)) - vec3(mat * (vec4(0,0,0,0)))) };
				// place a Projectile with the calculated direction in the world
				renderer->getWorld()->add<Projectile>(spaceShip->getPos(), direction);
			}
		}
		else spacePressed = false;

		// update the world
		worldManager->update(dt, spaceShip->getLocation(), spaceShip->getCamPos());

		// exit loop if window is closed or escape-key is pressed
		if (glfwGetKey(renderer->window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(renderer->window))
			renderer->exit();

		if (dt < 20)
			std::this_thread::sleep_for(milliseconds(20 - dt));
		});

	// main loop
	Renderer::loop(renderer);

}
