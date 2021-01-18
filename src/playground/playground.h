#ifndef PLAYGROUND_H
#define PLAYGROUND_H

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <memory>
#include "gameUtils.h"

shared_ptr<World> world;
shared_ptr<Renderer> renderer;

shared_ptr<SpaceShip> spaceShip;
shared_ptr<WorldManager> worldManager;

bool spacePressed{ false };

int main(void); //<<< main function, called at startup

#endif
