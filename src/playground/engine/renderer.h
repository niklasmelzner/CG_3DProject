#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <memory>
#include <map>
#include <GL/glew.h>
#include <glfw3.h>
#include "world.h"
#include "light.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

namespace eng {

	using namespace std;
	using namespace std::chrono;
	using namespace glm;

	class Renderer :public enable_shared_from_this<Renderer> {

		static bool initialized;
		static map<GLFWwindow*, shared_ptr<Renderer>> renderers;
		int width, height;
		const vec3 backgroundColor{ glm::vec3(0.0f,0.0f,0.0f) };
		shared_ptr<World> world;
		GLuint programId;
		mat4 projection, viewProjection, view;
		GLuint vertexArrayId;
		bool viewSupplierSet{false};
		function<mat4()> viewSupplier;
		shared_ptr<AttrIds> attrIds;
		float fov, zNear, zFar;
		bool updateListenerSet{false};
		function<void(shared_ptr<Renderer> renderer, long dt)>updateListener;
		bool running{ false };
		vec3 viewPos;
		bool viewPosSupplierSet{false};
		function<vec3()> viewPosSupplier;
		high_resolution_clock::time_point tLastUpdate = high_resolution_clock::now();

		void initProjection() {
			projection = perspective(fov, 1.0f * width / height, zNear, zFar);
		}

		void onWindowSizeCallback(GLFWwindow* window, int w, int h) {
			glfwMakeContextCurrent(window);
			glViewport(0, 0, width = w, height = h);
			initProjection();
		}

	public:
		GLFWwindow* window;

		Renderer(string title, int _width, int _height);

		inline void init() {
			renderers[window] = shared_from_this();
		}

		inline void setWorld(shared_ptr<World> _world) {
			world = _world;
		}

		inline shared_ptr<World> getWorld() {
			return world;
		}

		inline void setProgram(GLuint _programId) {
			programId = _programId;
		}

		inline void setViewSupplier(function<mat4()> _viewSupplier) {
			viewSupplier = _viewSupplier;
			viewSupplierSet = true;
		}

		inline void setViewPosSupplier(function<vec3()> _viewPosSupplier) {
			viewPosSupplier = _viewPosSupplier;
			viewPosSupplierSet = true;
		}

		inline void setViewPos(vec3 _viewPos) {
			viewPos = _viewPos;
			viewPosSupplierSet = false;
		}


		inline void setShaderAttrIds(shared_ptr<AttrIds> _attrIds) {
			attrIds = _attrIds;
		}

		inline void onUpdate(function<void(shared_ptr<Renderer> renderer, long dt)> listener) {
			updateListener = listener;
			updateListenerSet = true;
		}

		inline void exit() {
			running = false;
		}

		void setProjection(float _fov, float _zNear, float _zFar);

		void setView(vec3 cam, vec3 lookAt, vec3 up);

		void update();

		struct pass {
			template<typename ...T>pass(T...) {}

		};

		template<typename ...T> 
		static void loop(T ..._renderers) {
			vector<shared_ptr<Renderer>> renderers{};
			pass{ (renderers.push_back(_renderers), 1)... };

			for (shared_ptr<Renderer> renderer : renderers)renderer->running = true;

			int c = 0;
			while (renderers.size() > 0) {
				bool remove{ false };
				for (shared_ptr<Renderer> renderer : renderers) {
					if (renderer->running) renderer->update();
					else remove = true;
				}
				if (remove) {
					renderers.erase(remove_if(renderers.begin(), renderers.end(), [](shared_ptr<Renderer> renderer) {
						return !renderer->running;
						}), renderers.end());
				}

			}



		}

		~Renderer();

	};

}


#endif