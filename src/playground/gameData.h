#ifndef GAME_DATA_H
#define GAME_DATA_H

// Include GLM
#include <glm/glm.hpp>
#include <memory>
#include "model.h"



std::shared_ptr<ModelManager> modelManager;
std::shared_ptr<AnimationManager> animationManager;

glm::vec3 backgroundColor{ glm::vec3(0.0f,0.0f,0.0f) };

#endif


