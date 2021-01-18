#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <iostream>
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <regex>

using namespace std;
using namespace glm;

namespace eng {

	static string getParentPath(string path) {
		int ind = path.rfind('/');
		if (ind == string::npos)ind = path.rfind('\\');
		if (ind == string::npos)return "";
		return path.substr(0, ind);
	}

	class BMP {

	public:
		GLuint textureId;
		int width, height;

		BMP(string filename);

	};

	static shared_ptr<BMP> loadBMP(string filename) {
		return make_shared<BMP>(filename);
	}

	struct MTL {
		string name;
		vec3 ambient, diffuse, specular;
		shared_ptr<BMP> mapDiffuse;

		MTL() {};
		MTL(string _name) :name(_name) {};
	};

	class MTLLib {
		map<string, shared_ptr<MTL>> mtls;
	public:
		MTLLib(shared_ptr<vector<shared_ptr<MTL>>> _mtls) {
			for (shared_ptr<MTL> mtl : *_mtls) mtls[mtl->name] = mtl;
		}

		inline shared_ptr<MTL> get(string name) {
			return mtls[name];
		}
	};

	shared_ptr<MTLLib> loadMTLLib(string filename);

	class MTLConfig {
		map<int, shared_ptr<MTL>> mtlChanges{};
		
	public:
		vector<int> indices;

		MTLConfig(float r, float g, float b);
		MTLConfig() {}

		inline shared_ptr<MTL> get(int i) {
			return mtlChanges[i];
		}

		void set(int i, shared_ptr<MTL> mtl);
	};

	extern shared_ptr<MTLConfig> defaultMTLConfig;

	class OBJ :public enable_shared_from_this<OBJ> {
	public:
		const shared_ptr<vector<vec3>> vertices, normals;
		const shared_ptr<vector<vec2>> texCoords;
		const shared_ptr<MTLConfig> mtlConfig;

		OBJ(shared_ptr<vector<vec3>> _vertices, shared_ptr<vector<vec2>> _texCoords, shared_ptr<vector<vec3>> _normals) :
			OBJ(_vertices, _texCoords, _normals, nullptr) {}

		OBJ(shared_ptr<vector<vec3>> _vertices, shared_ptr<vector<vec2>> _texCoords, shared_ptr<vector<vec3>> _normals, shared_ptr<MTLConfig> _mtlConfig) :
			vertices(_vertices), texCoords(_texCoords), normals(_normals), mtlConfig(_mtlConfig) {}

	};
	
	shared_ptr<OBJ> loadOBJ(string filename, bool calcNormals = false);

}
#endif
