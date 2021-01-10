#ifndef GAME_DATA_H
#define GAME_DATA_H

// Include GLM
#include <glm/glm.hpp>
#include <memory>
#include "world.h"

std::shared_ptr<World> world;

static const int ASTEROID_COUNT{ 200 }, ASTEROID_SIZE_MIN{ 5 }, ASTEROID_SIZE_MAX{ 15 };
static const vec3 WORLD_SIZE{ 110 + ASTEROID_SIZE_MAX,110,110 };
static const vec3 WORLD_OFFSET{ WORLD_SIZE.x / 2 - ASTEROID_SIZE_MAX,0,0 };

static const float ASTEROID_DURATION_POPUP{ 300.0f };

static const int WO_TYPE_SPACE_SHIP{ 0 }, WO_TYPE_ASTEROID{ 1 };

static const glm::vec3 backgroundColor{ glm::vec3(0.0f,0.0f,0.0f) };

#endif


