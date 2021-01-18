#include "renderer.h"

namespace eng {

	bool Renderer::initialized{ false };
	map<GLFWwindow*, shared_ptr<Renderer>>Renderer::renderers{};

	Renderer::Renderer(string title, int _width, int _height) : width(_width), height(_height) {
		if (!initialized) {
			if (!glfwInit()) {
				fprintf(stderr, "Failed to initialize GLFW\n");
				std::exit(1);
			}
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		glfwMakeContextCurrent(window);

		if (!initialized) {
			initialized = true;

			glewExperimental = true;
			glewInit();

			glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
				shared_ptr<Renderer> renderer{ renderers[window] };
				renderer->onWindowSizeCallback(window, w, h);
				});
		}

		onWindowSizeCallback(window, width, height);

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.0f);

		glEnable(GL_DEPTH_TEST);

		glGenVertexArrays(1, &vertexArrayId);

		glBindVertexArray(vertexArrayId);
	}

	void Renderer::setProjection(float _fov, float _zNear, float _zFar) {
		fov = _fov;
		zNear = _zNear;
		zFar = _zFar;
		initProjection();
	}

	void Renderer::update() {
		auto now = high_resolution_clock::now();
		long elapsed = duration_cast<milliseconds>(now - tLastUpdate).count();
		tLastUpdate = now;

		long dt = elapsed;
		if (dt > 50) dt = 50;

		glfwMakeContextCurrent(window);

		if (updateListenerSet)
			updateListener(shared_from_this(), dt);
		else world->update(dt, vec3(0, 0, 0), vec3(-1, 0, 0));
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programId);

		view = viewSupplierSet ? viewSupplier() : view;
		viewProjection = projection * view;

		glUniformMatrix4fv(attrIds->vp, 1, GL_FALSE, &viewProjection[0][0]);
		glUniformMatrix4fv(attrIds->v, 1, GL_FALSE, &view[0][0]);

		glUniform3fv(attrIds->viewPos, 1, viewPosSupplierSet ? &viewPosSupplier()[0] : &viewPos[0]);

		world->updateMM(programId, attrIds);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void Renderer::setView(vec3 cam, vec3 lookAt, vec3 up) {
		view = glm::lookAt(
			cam,
			lookAt,
			up
		);
		viewSupplierSet = false;
	}

	Renderer::~Renderer() {
		glDeleteProgram(programId);
		glfwTerminate();
	}

}