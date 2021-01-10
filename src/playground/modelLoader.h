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

	BMP(string filename) {
		// external source http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/

		unsigned char header[54];
		FILE* file{ fopen(filename.c_str(),"rb") };

		assert(fread(header, 1, 54, file) == 54);

		assert(header[0] == 'B' && header[1] == 'M');

		int dataPos{ *(int*)&header[0x0A] }, imageSize{ *(int*)&header[0x22] };
		width = *(int*)&header[0x12];
		height = *(int*)&header[0x16];

		if (imageSize == 0) imageSize = width * height * 3;
		if (dataPos == 0) dataPos = 54;

		//unsigned char * data{ new unsigned char[imageSize] };

		shared_ptr<unsigned char>data{ shared_ptr<unsigned char>{ new unsigned char[imageSize], default_delete<unsigned char>() } };

		fread(data.get(), 1, imageSize, file);

		fclose(file);

		glGenTextures(1, &textureId);

		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data.get());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

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

	shared_ptr<MTL> get(string name) {
		return mtls[name];
	}
};

static shared_ptr<MTLLib> loadMTLLib(string filename) {
	// http://paulbourke.net/dataformats/mtl/
	ifstream file{ filename };
	shared_ptr<vector<shared_ptr<MTL>>> mtls{ make_shared< vector<shared_ptr<MTL>>>() };
	string line;
	shared_ptr<MTL> mtl{ nullptr };
	while (getline(file, line)) {
		const char* lineCh{ line.c_str() };
		if (line.length() == 0) {
			if (mtl != nullptr) {
				mtls->push_back(mtl);
				mtl = nullptr;
			}
		}
		else {
			regex prefix("[^ \n]+");
			smatch result;

			vector<string> pts;

			string::const_iterator start{ line.cbegin() }, end{ line.cend() };
			while (regex_search(start, end, result, prefix)) {
				pts.push_back(result[0]);
				start = result.suffix().first;
			}

			if (pts[0] == "newmtl") {
				assert(mtl == nullptr);
				mtl = make_shared<MTL>(pts[1]);
			}
			else if (pts[0] == "Ka")
				mtl->ambient = vec3(stof(pts[1]), stof(pts[2]), stof(pts[3]));
			else if (pts[0] == "Kd")
				mtl->diffuse = vec3(stof(pts[1]), stof(pts[2]), stof(pts[3]));
			else if (pts[0] == "Ks")
				mtl->specular = vec3(stof(pts[1]), stof(pts[2]), stof(pts[3]));
			else if (pts[0] == "map_Kd") {
				string fName = pts[1];

				string path = getParentPath(filename);
				path = path.length() == 0 ? fName : path + "/" + fName;

				shared_ptr<BMP> bmp{ loadBMP(path) };
				mtl->mapDiffuse = bmp;
			}
		}
	}
	if (mtl != nullptr)mtls->push_back(mtl);

	return make_shared<MTLLib>(mtls);
}

class MTLConfig {
	map<int, shared_ptr<MTL>> mtlChanges{};
public:
	MTLConfig(float r, float g, float b) {
		shared_ptr<MTL> mtl{ make_shared<MTL>() };
		mtl->ambient = vec3(r, g, b);
		mtl->diffuse = vec3(0.2, 0.2, 0.2);
		mtl->specular = vec3(0.1, 0.1, 0.1);
		set(0, mtl);
	}
	MTLConfig() {}

	vector<int> indices;

	void set(int i, shared_ptr<MTL> mtl) {
		mtlChanges[i] = mtl;
		indices.push_back(i);
		sort(indices.begin(), indices.end());
	}

	shared_ptr<MTL> get(int i) {
		return mtlChanges[i];
	}
};

static shared_ptr<MTLConfig> defaultMTLConfig{ make_shared<MTLConfig>(0.2,0.2,0.2) };

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
/*
regex regPrefix("[^ \n]+"), regFace("([0-9.]*)/([0-9.]*)/([0-9.]*)");
	while (getline(file, line)) {
		if (line.length() > 0) {
			smatch result;

			vector<string> pts;

			string::const_iterator start{ line.cbegin() }, end{ line.cend() };
			while (regex_search(start, end, result, regPrefix)) {
				pts.push_back(result[0]);
				start = result.suffix().first;
			}

			if (pts[0] == "v") {
				obj_vertices.push_back(vec3(stof(pts[1]), stof(pts[2]), stof(pts[3])));
			}
			else if (pts[0] == "vt") {
				obj_texCoords.push_back(vec2(stof(pts[1]), stof(pts[2])));
			}
			else if (pts[0] == "vn") {
				obj_normals.push_back(vec3(stof(pts[1]), stof(pts[2]), stof(pts[3])));
			}
			else if (pts[0] == "f") {
				vector<vec3> indices{};
				smatch result;
				for (int i = 1; i < pts.size(); i++) {
					regex_match(pts[i], result, regFace);
					indices.push_back(vec3(stof(result[0]), stof(result[1]), stof(result[2])));
				}
				for (int i = 0; i < indices.size() - 2; i++) {
					for (int j = 0; j < 3; j++) {
						vec3 ind{ indices[j == 0 ? 0 : i + j] };
						vertexIndices.push_back(ind.x);
						texIndices.push_back(ind.y);
						normalIndices.push_back(ind.z);
					}
				}
			}
			else if (pts[0] == "mtllib") {
				assert(mtllib == nullptr);
				string mtllibFilename;

				for (int i = 1; i < pts.size(); i++)mtllibFilename += (i != 1 ? " " : "") + pts[i];

				std::cout << mtllibFilename << "\n";

				string path = getParentPath(filename);
				path = path.length() == 0 ? mtllibFilename : path + "/" + mtllibFilename;

				mtllib = loadMTLLib(path);
				mtlConfig = make_shared<MTLConfig>();
			}
			else if (pts[0] == "usemtl") {
				string mtlName;

				for (int i = 1; i < pts.size(); i++)mtlName += (i != 1 ? " " : "") + pts[i];

				std::cout << mtlName << "\n";

				shared_ptr<MTL> mtl{ mtllib->get(mtlName) };
				assert(mtl != nullptr);
				mtlConfig->set(vertexIndices.size(), mtl);
			}
		}
	}


*/

static shared_ptr<OBJ> loadOBJ(string filename, bool calcNormals = false) {
	// external source http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
	vector<int> vertexIndices, texIndices, normalIndices;
	vector<vec3> obj_vertices{};
	vector<vec2> obj_texCoords{};
	vector<vec3> obj_normals{};

	shared_ptr<MTLLib> mtllib{ nullptr };

	shared_ptr<MTLConfig> mtlConfig{ nullptr };

	FILE* file{ fopen(filename.c_str(),"r") };
	while (true) {
		char lineHeader[128];
		if (fscanf(file, "%s", lineHeader) == EOF) break;

		if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex;
			fscanf(file, "%f %f %f \n", &vertex.x, &vertex.y, &vertex.z);
			obj_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			vec2 texCoord;
			fscanf(file, "%f %f\n", &texCoord.x, &texCoord.y);
			obj_texCoords.push_back(texCoord);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 normal;
			fscanf(file, "%f %f %f \n", &normal.x, &normal.y, &normal.z);
			obj_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			vector<vec3> indices{};
			while (true) {
				int vIndex, tIndex, nIndex;
				fscanf(file, "%d/%d/%d", &vIndex, &tIndex, &nIndex);

				indices.push_back(vec3(vIndex, tIndex, nIndex));

				char ch;
				fscanf(file, "%c", &ch);
				if (ch == '\n') break;
			}
			for (int i = 0; i < indices.size() - 2; i++) {
				for (int j = 0; j < 3; j++) {
					vec3 ind{ indices[j == 0 ? 0 : i + j] };
					vertexIndices.push_back(ind.x);
					texIndices.push_back(ind.y);
					normalIndices.push_back(ind.z);
				}
			}
		}
		else if (strcmp(lineHeader, "mtllib") == 0) {
			assert(mtllib == nullptr);
			std::string mtllibFilename;
			while (true) {
				char tmpName[128];

				fscanf(file, "%s", tmpName);
				if (mtllibFilename.length() > 0)
					mtllibFilename += " ";
				mtllibFilename += tmpName;

				char ch;
				fscanf(file, "%c", &ch);
				if (ch == '\n') break;

			}


			string path = getParentPath(filename);
			path = path.length() == 0 ? mtllibFilename : path + "/" + mtllibFilename;

			mtllib = loadMTLLib(path);
			mtlConfig = make_shared<MTLConfig>();
		}
		else if (strcmp(lineHeader, "usemtl") == 0) {
			char mtlName[128];
			fscanf(file, "%s", mtlName);
			shared_ptr<MTL> mtl{ mtllib->get(mtlName) };
			assert(mtl != nullptr);
			mtlConfig->set(vertexIndices.size(), mtl);
		}
	}
	// external source end

	vector<vec3> vertices{ };
	vector<vec2> texCoords{  };
	vector<vec3> normals{ };

	if (calcNormals) {
		vector<vec3> vertexNormals{ obj_vertices.size() };
		for (int i = 0; i < vertexIndices.size(); i += 3) {
			int i0{ vertexIndices[i] - 1 };
			int i1{ vertexIndices[i + 1] - 1 };
			int i2{ vertexIndices[i + 2] - 1 };
			vec3 v0{ obj_vertices[i0] };
			vec3 v1{ obj_vertices[i1] };
			vec3 v2{ obj_vertices[i2] };

			vec3 n{ cross(v1 - v0, v2 - v0) };

			vertexNormals[i0] += n;
			vertexNormals[i1] += n;
			vertexNormals[i2] += n;
		}

		for (int i = 0; i < vertexNormals.size(); i++)
			vertexNormals[i] = normalize(vertexNormals[i]);

		for (int i = 0; i < vertexIndices.size(); i++)
			normals.push_back(vertexNormals[vertexIndices[i] - 1]);
	}


	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vIndex{ vertexIndices[i] };
		int tIndex{ texIndices[i] };
		int nIndex{ normalIndices[i] };

		vertices.push_back(obj_vertices[vIndex - 1]);
		texCoords.push_back(obj_texCoords[tIndex - 1]);
		if (!calcNormals)
			normals.push_back(obj_normals[nIndex - 1]);
	}

	if (mtlConfig.get() == nullptr)
		return make_shared<OBJ>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec2>>(texCoords), make_shared<vector<vec3>>(normals));
	else
		return make_shared<OBJ>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec2>>(texCoords), make_shared<vector<vec3>>(normals), mtlConfig);
}

#endif
