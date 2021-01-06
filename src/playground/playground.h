#ifndef PLAYGROUND_H
#define PLAYGROUND_H

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <memory>
#include "model.h"
#include "gameUtils.h"

//some global variables for handling the vertex buffer
GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint vertexbuffer_size;


//program ID of the shaders, required for handling the shaders with OpenGL
GLuint programID;

int windowWidth, windowHeight;

//global variables to handle the MVP matrix
GLuint VPMatrixID;
GLuint MMatrixID,VMatrixID;
glm::mat4 VP,V;

float curr_x;
float curr_y;
float curr_angle;

shared_ptr<SpaceShip> spaceShip;

int main(void); //<<< main function, called at startup
void updateAnimationLoop(); //<<< updates the animation loop
bool initializeWindow(); //<<< initializes the window using GLFW and GLEW
bool initializeVPTransformation();
bool initializeVertexbuffer(); //<<< initializes the vertex buffer array and binds it OpenGL
bool cleanupVertexbuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow(); //<<< Closes the OpenGL window and terminates GLFW

#endif
