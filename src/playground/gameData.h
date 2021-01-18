#ifndef GAME_DATA_H
#define GAME_DATA_H

// Include GLM
#include <glm/glm.hpp>
#include <memory>
#include "engine/renderer.h"
#include "engine/world.h"

static const int ASTEROID_COUNT{ 200 }, ASTEROID_SIZE_MIN{ 5 }, ASTEROID_SIZE_MAX{ 20 };
static const vec3 WORLD_SIZE{ 110 + ASTEROID_SIZE_MAX,110,110 };
static const vec3 WORLD_OFFSET{ WORLD_SIZE.x / 2 - ASTEROID_SIZE_MAX,0,0 };
static const int MAX_COLLISIONS{5};

static const float ASTEROID_DURATION_POPUP{ 300.0f };
static const float ASTEROID_DURATION_EXPLODE{ 200.0f };
static const float SPACESHIP_DURATION_EXPLODE{ 1000.0f };

static const int WO_TYPE_SPACE_SHIP{ 0 }, WO_TYPE_ASTEROID{ 1 }, WO_TYPE_PROJECTILE{ 2 }, WO_TYPE_EXPLOSION{ 3 };

static const vec3 backgroundColor{ vec3(0.0f,0.0f,0.0f) };


#endif


