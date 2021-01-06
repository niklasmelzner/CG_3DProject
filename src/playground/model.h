#ifndef MODEL_H
#define MODEL_H

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include "parse_stl.h"
#include <fstream>
#include <regex>

using namespace std;
using namespace glm;

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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

};

static shared_ptr<BMP> loadBMP(string filename) {
	return make_shared<BMP>(filename);
}

class BMPView {
public:
	const float x, y, w, h;
	const shared_ptr<BMP> bmp;

	BMPView(shared_ptr<BMP> _bmp, float _x, float _y, float _w, float _h) :
		bmp(_bmp), x(_x), y(_y), w(_w), h(_h) {}

};

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
		for (shared_ptr<MTL> mtl : *_mtls) {
			std::cout << mtl->name << ": " << mtl->mapDiffuse << "\n";
			mtls[mtl->name] = mtl;
		}
	}

	shared_ptr<MTL> get(string name) {
		return mtls[name];
	}
};

static void loadSTLFile(vector< vec3 >& vertices, vector< vec3 >& normals, string stl_file_name) {
	stl::stl_data info = stl::parse_stl(stl_file_name);
	vector<stl::triangle> triangles = info.triangles;
	for (int i = 0; i < info.triangles.size(); i++) {
		stl::triangle t = info.triangles[i];
		vec3 triangleNormal = vec3(t.normal.x,
			t.normal.y,
			t.normal.z);
		//add vertex and normal for point 1:
		vertices.push_back(vec3(t.v1.x, t.v1.y, t.v1.z));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 2:
		vertices.push_back(vec3(t.v2.x, t.v2.y, t.v2.z));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 3:
		vertices.push_back(vec3(t.v3.x, t.v3.y, t.v3.z));
		normals.push_back(triangleNormal);
	}
}

static string getDirFromPath(string path) {
	int ind = path.rfind('/');
	if (ind == string::npos)ind = path.rfind('\\');
	if (ind == string::npos)return "";
	return path.substr(0, ind);
}

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

				string path = getDirFromPath(filename);
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
	vector<int> indices;

	void set(int i, shared_ptr<MTL> mtl) {
		mtlChanges[i] = mtl;
		indices.push_back(i);
		std::sort(indices.begin(), indices.end());
	}

	shared_ptr<MTL> get(int i) {
		return mtlChanges[i];
	}
};

class OBJ {
public:
	const shared_ptr<vector<vec3>> vertices, normals;
	const shared_ptr<vector<vec2>> texCoords;
	const shared_ptr<MTLConfig> mtlConfig;

	OBJ(shared_ptr<vector<vec3>> _vertices, shared_ptr<vector<vec2>> _texCoords, shared_ptr<vector<vec3>> _normals) :
		OBJ(_vertices, _texCoords, _normals, nullptr) {}

	OBJ(shared_ptr<vector<vec3>> _vertices, shared_ptr<vector<vec2>> _texCoords, shared_ptr<vector<vec3>> _normals, shared_ptr<MTLConfig> _mtlConfig) :
		vertices(_vertices), texCoords(_texCoords), normals(_normals), mtlConfig(_mtlConfig) {}

};

static shared_ptr<OBJ> loadOBJ(string filename) {
	// external source http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
	vector<int> vertexIndices, texIndices, normalIndices;
	vector<vec3> obj_vertices{};
	vector<vec2> obj_texCoords{};
	vector<vec3> obj_normals{};

	FILE* file{ fopen(filename.c_str(),"r") };
	shared_ptr<MTLLib> mtllib{ nullptr };

	shared_ptr<MTLConfig> mtlConfig{ nullptr };
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
			char mtllibFilename[128];

			fscanf(file, "%s", mtllibFilename);

			string path = getDirFromPath(filename);
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

	vector<vec3> vertices;
	vector<vec2> texCoords;
	vector<vec3> normals;

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vIndex{ vertexIndices[i] };
		int tIndex{ texIndices[i] };
		int nIndex{ normalIndices[i] };

		vertices.push_back(obj_vertices[vIndex - 1]);
		texCoords.push_back(obj_texCoords[tIndex - 1]);
		normals.push_back(obj_normals[nIndex - 1]);
	}

	if (mtlConfig.get() == nullptr)
		return make_shared<OBJ>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec2>>(texCoords), make_shared<vector<vec3>>(normals));
	else
		return make_shared<OBJ>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec2>>(texCoords), make_shared<vector<vec3>>(normals), mtlConfig);
}

class AnimatedObject {
public:

	virtual void precalculateStep(long dt) = 0;

	virtual void submitStep() = 0;

};

class ModelBase {
	shared_ptr<GLfloat> vertex_buffer_data;
	shared_ptr<GLfloat> normal_buffer_data;
	shared_ptr<GLfloat> color_buffer_data;
	shared_ptr<GLuint> index_buffer_data;
	shared_ptr<GLfloat> texture_buffer_data;
	shared_ptr<MTLConfig> mtlConfig;
	bool hasMTLConfig;
	const int vertexCount, indexCount;
	int vertexInsertIndex{ 0 }, normalInsertIndex{ 0 }, indexInsertIndex{ 0 };
	GLuint vertexbuffer, colorbuffer, normalbuffer, indexbuffer, texturebuffer;
	bool vertexDataChanged{ true }, indexDataChanged{ false };
	int normalMode;
	bool calcNormals;
	shared_ptr<BMPView> bmpView;

	void addTriangleFace(vec3 p0, vec3 p1, vec3 p2,
		vec3 c0, vec3 c1, vec3 c2) {
		int ind{ vertexInsertIndex };
		addTriangle(ind, ind + 1, ind + 2);

		addVertex(p0, c0);
		addVertex(p1, c1);
		addVertex(p2, c2);
	}

	void exCalcNormals() {
		for (int i = 0; i < vertexInsertIndex * 3; i++)normal_buffer_data.get()[i] = 0;

		for (int i = 0; i < indexInsertIndex; i += 3)
		{
			GLuint ind0 = index_buffer_data.get()[i];
			GLuint ind1 = index_buffer_data.get()[i + 1];
			GLuint ind2 = index_buffer_data.get()[i + 2];

			vec3 v0{
				vertex_buffer_data.get()[ind0 * 3],
				vertex_buffer_data.get()[ind0 * 3 + 1],
				vertex_buffer_data.get()[ind0 * 3 + 2]
			};
			vec3 v1{
				vertex_buffer_data.get()[ind1 * 3],
				vertex_buffer_data.get()[ind1 * 3 + 1],
				vertex_buffer_data.get()[ind1 * 3 + 2]
			};
			vec3 v2{
				vertex_buffer_data.get()[ind2 * 3],
				vertex_buffer_data.get()[ind2 * 3 + 1],
				vertex_buffer_data.get()[ind2 * 3 + 2]
			};
			vec3 n{ normalize(cross(v1 - v0,v2 - v0)) };
			for (int j = 0; j < 3; j++)
			{
				normal_buffer_data.get()[ind0 * 3 + j] += n[j];
				normal_buffer_data.get()[ind1 * 3 + j] += n[j];
				normal_buffer_data.get()[ind2 * 3 + j] += n[j];
			}
		}

		for (int i = 0; i < vertexInsertIndex; i++) {
			vec3 v{
				normal_buffer_data.get()[i * 3],
				normal_buffer_data.get()[i * 3 + 1],
				normal_buffer_data.get()[i * 3 + 2]
			};
			v = normalize(v);
			normal_buffer_data.get()[i * 3] = v.x;
			normal_buffer_data.get()[i * 3 + 1] = v.y;
			normal_buffer_data.get()[i * 3 + 2] = v.z;
		}
	}

public:
	bool hasTexture;

	ModelBase(int _indexCount, int _vertexCount) :ModelBase(_indexCount, _vertexCount, true) {

	}

	ModelBase(int _indexCount, int _vertexCount, shared_ptr<MTLConfig> _mtlConfig) :
		indexCount(_indexCount), vertexCount(_vertexCount), calcNormals(false), mtlConfig(_mtlConfig) {

		vertex_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &vertexbuffer);

		normal_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &normalbuffer);

		index_buffer_data = std::shared_ptr<GLuint>{ new GLuint[indexCount],std::default_delete<GLuint[]>() };
		glGenBuffers(1, &indexbuffer);

		texture_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 2],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &texturebuffer);

		hasMTLConfig = true;
		hasTexture = false;
	}

	ModelBase(int _indexCount, int _vertexCount, bool _calcNormals) :
		indexCount(_indexCount), vertexCount(_vertexCount), calcNormals(_calcNormals) {

		vertex_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &vertexbuffer);

		normal_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &normalbuffer);

		index_buffer_data = std::shared_ptr<GLuint>{ new GLuint[indexCount],std::default_delete<GLuint[]>() };
		glGenBuffers(1, &indexbuffer);

		color_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &colorbuffer);

		hasMTLConfig = false;
		hasTexture = false;
	}

	ModelBase(int _indexCount, int _vertexCount, shared_ptr<BMPView> _bmpView) :ModelBase(_indexCount, _vertexCount, _bmpView, false) {}

	ModelBase(int _indexCount, int _vertexCount, shared_ptr<BMPView> _bmpView, bool _calcNormals) :
		indexCount(_indexCount), vertexCount(_vertexCount), calcNormals(_calcNormals), bmpView(_bmpView) {
		vertex_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &vertexbuffer);

		normal_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &normalbuffer);

		index_buffer_data = std::shared_ptr<GLuint>{ new GLuint[indexCount],std::default_delete<GLuint[]>() };
		glGenBuffers(1, &indexbuffer);

		texture_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 2],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &texturebuffer);
		hasMTLConfig = false;
		hasTexture = true;
	}

	void addVertex(vec3 v, vec3 n, vec2 t) {
		assert(hasTexture || hasMTLConfig);
		vertex_buffer_data.get()[vertexInsertIndex * 3] = v.x;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 1] = v.y;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 2] = v.z;
		normal_buffer_data.get()[vertexInsertIndex * 3] = n.x;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 1] = n.y;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 2] = n.z;
		texture_buffer_data.get()[vertexInsertIndex * 2] = t.x;// bmpView->x + t.x * bmpView->w;
		texture_buffer_data.get()[vertexInsertIndex * 2 + 1] = t.y;// bmpView->y + t.y * bmpView->h;
		vertexInsertIndex++;
		vertexDataChanged = true;
	}

	void addVertex(vec3 v, vec3 n, vec3 c) {
		assert(!hasTexture && !hasMTLConfig);
		vertex_buffer_data.get()[vertexInsertIndex * 3] = v.x;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 1] = v.y;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 2] = v.z;
		normal_buffer_data.get()[vertexInsertIndex * 3] = n.x;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 1] = n.y;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 2] = n.z;
		color_buffer_data.get()[vertexInsertIndex * 3] = c.r;
		color_buffer_data.get()[vertexInsertIndex * 3 + 1] = c.g;
		color_buffer_data.get()[vertexInsertIndex * 3 + 2] = c.b;
		vertexInsertIndex++;
		vertexDataChanged = true;
	}

	void addVertex(vec3 v, vec3 c) {
		assert(!hasTexture);
		vertex_buffer_data.get()[vertexInsertIndex * 3] = v.x;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 1] = v.y;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 2] = v.z;
		color_buffer_data.get()[vertexInsertIndex * 3] = c.r;
		color_buffer_data.get()[vertexInsertIndex * 3 + 1] = c.g;
		color_buffer_data.get()[vertexInsertIndex * 3 + 2] = c.b;
		vertexInsertIndex++;
		vertexDataChanged = true;
	}

	void addTriangle(GLuint i0, GLuint i1, GLuint i2) {
		index_buffer_data.get()[indexInsertIndex++] = i0;
		index_buffer_data.get()[indexInsertIndex++] = i1;
		index_buffer_data.get()[indexInsertIndex++] = i2;
		indexDataChanged = true;
	}

	/*void submitFaces() {
		int normalInsertIndex = this->normalInsertIndex;
		std::cout << normalInsertIndex << ":" << vertexInsertIndex << "\n";
		int nCount = vertexInsertIndex - normalInsertIndex;
		std::vec3 normals{ new vec3[nCount],std::default_delete<vec3[]>() };


		for (int i = 0; i < nCount/3; i++) {
			vec3v0{ getVertex(normalInsertIndex + i * 3) };
			vec3v1{ getVertex(normalInsertIndex + i * 3 + 1) };
			vec3v2{ getVertex(normalInsertIndex + i * 3 + 2) };

			vec3 n{
				vec3(normalize(cross(*v1 - *v0, *v2 - *v0)))
			};

			normals.get()[i] = n;
		}
	}*/

	vec3 getVertex(int index) {
		return vec3(
			vertex_buffer_data.get()[index * 3],
			vertex_buffer_data.get()[index * 3 + 1],
			vertex_buffer_data.get()[index * 3 + 2]
		);
	}

	void addFace(vec3 p0, vec3 p1, vec3 p2, vec3 color) {
		addFace(p0, p1, p2, color, color, color);
	}

	void addFace(vec3 p0, vec3 p1, vec3 p2,
		vec3 c1, vec3 c2, vec3 c3) {
		addTriangleFace(p0, p1, p2, c1, c2, c3);
	}

	void loadMTL(shared_ptr<MTL> mtl, GLuint programID, GLuint hasTextureId) {
		bool hasTexture = mtl->mapDiffuse.get() != nullptr;
		if (hasTexture) {
			glBindTexture(GL_TEXTURE_2D, mtl->mapDiffuse->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glUniform1f(hasTextureId, 1.0);
		}
		else glUniform1f(hasTextureId, 0.0);
		glUniform3f(glGetUniformLocation(programID, "material.ambient"), mtl->ambient.r, mtl->ambient.g, mtl->ambient.b);
		glUniform3f(glGetUniformLocation(programID, "material.diffuse"), mtl->diffuse.r, mtl->diffuse.g, mtl->diffuse.b);
		glUniform3f(glGetUniformLocation(programID, "material.specular"), mtl->specular.r, mtl->specular.g, mtl->specular.b);
	}

	void update(GLuint programID, GLuint hasTextureId, GLuint hasMatId) {
		if (vertexDataChanged) {
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 3, vertex_buffer_data.get(), GL_STATIC_DRAW);

			if (calcNormals)exCalcNormals();

			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 3, normal_buffer_data.get(), GL_STATIC_DRAW);

			if (hasTexture || hasMTLConfig) {
				glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 2, texture_buffer_data.get(), GL_STATIC_DRAW);
			}
			else {
				glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 3, color_buffer_data.get(), GL_STATIC_DRAW);
			}

			vertexDataChanged = false;
		}
		if (indexDataChanged) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, index_buffer_data.get(), GL_STATIC_DRAW);

			indexDataChanged = false;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		if (!hasMTLConfig) {
			if (hasTexture) {
				glEnableVertexAttribArray(3);
				glBindTexture(GL_TEXTURE_2D, bmpView->bmp->textureId);
				glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glUniform1f(hasTextureId, 1.0);
			}
			else {
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glUniform1f(hasTextureId, 0.0);
			}
		}

		if (hasMTLConfig) {
			glUniform1f(hasMatId, 1.0);
			vector<int>* indices{ &mtlConfig->indices };
			for (int i = 0, size = indices->size(); i < size; i++) {
				loadMTL(mtlConfig->get(indices->at(i)), programID, hasTextureId);
				bool textureActive = mtlConfig->get(indices->at(i))->mapDiffuse.get() != nullptr;

				if (textureActive)glEnableVertexAttribArray(3);

				int start{ indices->at(i) };
				int end{ i == size - 1 ? indexCount : indices->at(i + 1) };
				glDrawElementsBaseVertex(GL_TRIANGLES, end-start, GL_UNSIGNED_INT, 0, start);
				if (textureActive)glDisableVertexAttribArray(3);
			}
			std::cout << "\n";
			//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			glDisableVertexAttribArray(3);
		}
		else {
			glUniform1f(hasMatId, 0.0);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			glDisableVertexAttribArray(hasTexture ? 3 : 2);
		}

	}

	~ModelBase() {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &colorbuffer);
		glDeleteBuffers(1, &indexbuffer);
		glDeleteBuffers(1, &normalbuffer);
	}

};

class ModelInstance : public AnimatedObject {
	mat4 transformation{ mat4(1.0f) };

	vec3 color{ 0.8f,0.0f,0.0f };

public:
	shared_ptr<ModelBase> model;

	ModelInstance(shared_ptr<ModelBase> _model) :model(_model) {}

	long dt;
	void precalculateStep(long dt) {
		this->dt = dt;
	};

	void submitStep() {
		rotate(0.003, vec3(1, -1, 1));
	};

	void setColor(vec3 color) {
		this->color = color;
	}

	void update(GLuint programID, GLuint mMatrixId, GLuint hasTextureId, GLuint hasMatId) {
		glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &transformation[0][0]);
		model->update(programID, hasTextureId, hasMatId);
	}

	inline void translate(vec3 vec) {
		translate(vec.x, vec.y, vec.z);
	}

	void rotate(float angle, vec3 axis) {
		transformation = glm::rotate(transformation, angle, axis);
	}

	void translate(float x, float y, float z) {
		transformation = glm::translate(transformation, vec3{ x,y,z });
	}

	void scale(float x, float y, float z) {
		transformation = glm::scale(transformation, vec3{ x,y,z });
	}

	void transform(mat4 mat) {
		transformation = mat * transformation;
	}

	void setTranslation(vec3 vec) {

		transformation[3][0] = vec.x;
		transformation[3][1] = vec.y;
		transformation[3][2] = vec.z;
	}

};

static const float PI{ 3.1415926535897932384626433832 };

class VNObject :public ModelBase {

public:
	VNObject(shared_ptr<vector<vec3>> vertices, shared_ptr<vector<vec3>> normals) :ModelBase(vertices->size(), vertices->size(), false) {

		vec3 color{ vec3(0.8,0.8,0.8) };
		for (int i = 0; i < vertices->size(); i++) {
			addVertex(vec3(vertices->at(i)), vec3(normals->at(i)), color);
			if (i % 3 == 0) addTriangle(i, i + 1, i + 2);
		}
	}

};

class IndexedObject : public ModelBase {

public:
	IndexedObject(shared_ptr<vector<vec3>> vertices, shared_ptr<vector<vec3>> normals, shared_ptr<vector<int>> indices) :ModelBase(indices->size(), vertices->size()) {
		vec3 color{ vec3(0.8,0.8,0.8) };
		for (int i = 0; i < vertices->size(); i++) addVertex(vertices->at(i), normals->at(i), color);

		addIndices(indices);
	}

	IndexedObject(shared_ptr<vector<vec3>> vertices, shared_ptr<vector<int>> indices) :ModelBase(indices->size(), vertices->size()) {

		vec3 color{ vec3(0,1,0) };
		for (int i = 0; i < vertices->size(); i++)addVertex(vertices->at(i), color);

		addIndices(indices);
	}

	void addIndices(shared_ptr<vector<int>> indices) {
		for (int i = 0; i < indices->size() / 3; i++) addTriangle(indices->at(i * 3), indices->at(i * 3 + 1), indices->at(i * 3 + 2));
	}
};

static shared_ptr<ModelBase> createSphere(int accuracy) {
	vec3 top{ vec3(0,0.5,0) };
	vec3 bottom{ vec3(0,-0.5,0) };

	shared_ptr<vector<vec3>> vertices{ make_shared<vector<vec3>>() };
	shared_ptr<vector<int>> indices{ make_shared<vector<int>>() };

	vertices->push_back(top);
	vertices->push_back(bottom);

	for (int i = 1; i < accuracy; i++)
	{
		float a{ PI * i / accuracy };
		for (float j = (i % 2 == 0) ? 0 : 0.5; j < accuracy * 2; j++)
		{
			float b{ PI * j / accuracy };
			vertices->push_back(vec3{ sin(b) * sin(a) / 2.0f, cos(a) / 2.0f, cos(b) * sin(a) / 2.0f });
		}
	}

	for (int i = 0; i < accuracy * 2; i++)
	{
		indices->push_back(0);
		indices->push_back(2 + i);
		indices->push_back(2 + (i + 1) % (accuracy * 2));

		indices->push_back(1);
		indices->push_back(2 + (accuracy - 2) * accuracy * 2 + (i + 1) % (accuracy * 2));
		indices->push_back(2 + (accuracy - 2) * accuracy * 2 + i);
	}

	for (int i = 0; i < accuracy - 2; i++)
	{
		for (int j = 0; j < accuracy * 2; j++)
		{

			if (i % 2 == 0) {
				indices->push_back(2 + i * accuracy * 2 + j);
				indices->push_back(2 + (i + 1) * accuracy * 2 + j);
				indices->push_back(2 + (i + 1) * accuracy * 2 + (j + 1) % (accuracy * 2));

				indices->push_back(2 + i * accuracy * 2 + (j + 1) % (accuracy * 2));
				indices->push_back(2 + i * accuracy * 2 + j);
				indices->push_back(2 + (i + 1) * accuracy * 2 + (j + 1) % (accuracy * 2));
			}
			else {
				indices->push_back(2 + i * accuracy * 2 + (j + 1) % (accuracy * 2));
				indices->push_back(2 + (i + 1) * accuracy * 2 + j);
				indices->push_back(2 + (i + 1) * accuracy * 2 + (j + 1) % (accuracy * 2));

				indices->push_back(2 + i * accuracy * 2 + (j + 1) % (accuracy * 2));
				indices->push_back(2 + i * accuracy * 2 + j);
				indices->push_back(2 + (i + 1) * accuracy * 2 + j);
			}
		}
	}

	return make_shared<IndexedObject>(vertices, indices);

}

class Sphere : public ModelBase {

public:
	static void construct(int accuracy) {
		shared_ptr<vector<vec3>> vertices{ make_shared<vector<vec3>>() };
		shared_ptr<vector<int>> indices{ make_shared<vector<int>>() };

		vec3 top{ vec3(0,0.5,0) };
		vec3 bottom{ vec3(0,-0.5,0) };

		for (int i = 1; i < accuracy; i++)
		{
			for (int j = 1; j < accuracy * 2; j++)
			{

			}
		}


	}

	Sphere() : ModelBase(12, 12) {



		/*float f = 0.28867513459;
		vec3 p0{ vec3(-f ,-f ,-f) };
		vec3 p1{ vec3(f ,f , -f) };
		vec3 p2{ vec3(-f , f , f) };
		vec3 p3{ vec3(f ,-f ,  f) };

		shared_ptr<vector<vec3>> vertices1{ make_shared<vector<vec3>>() };
		shared_ptr<vector<vec3>> vertices2{ make_shared<vector<vec3>>() };

		vec3 c = cross(*p1 - *p0, *p3 - *p0);

		addSphereFace(vertices1, p0, p1, p3);
		addSphereFace(vertices1, p0, p2, p1);
		addSphereFace(vertices1, p0, p3, p2);
		addSphereFace(vertices1, p1, p2, p3);

		for (int i = 0; i < accuracy; i++) {
			if (i % 2 == 0) iterate(vertices1, vertices2);
			else iterate(vertices2, vertices1);
		}



		shared_ptr<vector<vec3>> end{ accuracy % 2 == 1 ? vertices2 : vertices1 };

		shared_ptr<vector<vec3>> colors{ generateColors(end->size()) };

		for (int i = 0; i < end->size(); i += 3) {
			addFace(
				end->at(i),
				end->at(i + 1),
				end->at(i + 2),
				colors->at(i),
				colors->at(i + 1),
				colors->at(i + 2)
			);
		}

		//submitFaces();*/
	}

	void iterate(shared_ptr<vector<vec3>> source, shared_ptr<vector<vec3>> target) {
		target->clear();

		float d = 0.8;

		for (int i = 0; i < source->size(); i += 3) {
			vec3 p0{ source->at(i) };
			vec3 p1{ source->at(i + 1) };
			vec3 p2{ source->at(i + 2) };

			vec3 pm0{ p0 + p1 };
			vec3 pm1{ p1 + p2 };
			vec3 pm2{ p2 + p0 };


			/*vec3 center{*p0 + *p1 + *p2};

			pm0 = center + (pm0-center)*d;
			pm1 = center + (pm1-center) * d;
			pm2 = center + (pm2-center) * d;*/

			vec3 pM0{ vec3(pm0 * (0.5f / length(pm0))) };
			vec3 pM1{ vec3(pm1 * (0.5f / length(pm1))) };
			vec3 pM2{ vec3(pm2 * (0.5f / length(pm2))) };

			addSphereFace(target, pM0, pM1, pM2);

			addSphereFace(target, pM1, p2, pM2);
			addSphereFace(target, pM0, pM2, p0);
			addSphereFace(target, pM0, p1, pM1);

		}
	}

	shared_ptr<vector<vec3>> generateColors(int size) {
		shared_ptr<vector<vec3>> colors{ make_shared<vector<vec3>>() };
		for (int i = 0; i < size; i++)
		{
			float r = 0.5 + (rand() % 20) / 40.0;
			float g = 0.5 + (rand() % 20) / 40.0;
			float b = 0.5 + (rand() % 20) / 40.0;

			r = g = b = 0.8f;
			r = 0;
			g = b = 0;
			g = 0.8f;

			colors->push_back(vec3(r, g, b));
			colors->push_back(vec3(r, g, b));
			colors->push_back(vec3(r, g, b));
		}

		return colors;


	}

	void addSphereFace(shared_ptr<vector<vec3>> vertices, vec3 p0, vec3 p1, vec3 p2) {
		vertices->push_back(p0);
		vertices->push_back(p1);
		vertices->push_back(p2);
	}

};

class Cube : public ModelBase {

	vec3 color;

public:
	Cube(float r, float g, float b) :ModelBase(36, 36), color{ vec3(r,g,b) } {

		vec3 p0{ vec3(-0.5 ,-0.5 ,-0.5) };
		vec3 p1{ vec3(0.5 ,-0.5 , -0.5) };
		vec3 p2{ vec3(-0.5 , 0.5 , -0.5) };
		vec3 p3{ vec3(-0.5 ,-0.5 ,  0.5) };
		vec3 p4{ vec3(-0.5 , 0.5 ,  0.5) };
		vec3 p5{ vec3(0.5 ,-0.5 , 0.5) };
		vec3 p6{ vec3(0.5 , 0.5 , -0.5) };
		vec3 p7{ vec3(0.5 , 0.5 ,  0.5) };

		addRectangle(p0, p2, p6, p1);
		addRectangle(p3, p5, p7, p4);
		addRectangle(p0, p3, p4, p2);
		addRectangle(p1, p6, p7, p5);
		addRectangle(p0, p1, p5, p3);
		addRectangle(p2, p4, p7, p6);
	}

	void addRectangle(vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
		addFace(p0, p1, p2, color);
		addFace(p0, p2, p3, color);
	}
};

class AnimationManager {

	shared_ptr<vector<shared_ptr<AnimatedObject>>> animatedObjects{ make_shared<vector<shared_ptr<AnimatedObject>>>() };

public:

	inline void add(shared_ptr<AnimatedObject> object) {
		animatedObjects->push_back(object);
	}

	void update(long dt) {
		for (shared_ptr<AnimatedObject> model : *animatedObjects.get()) model->precalculateStep(dt);
		for (shared_ptr<AnimatedObject> model : *animatedObjects.get()) model->submitStep();

	}

};

class ModelManager {
	shared_ptr<vector<shared_ptr<ModelInstance>>> models{ make_shared <vector<shared_ptr<ModelInstance>>>() };

public:

	inline void add(shared_ptr<ModelInstance> model) {
		models->push_back(model);
	}

	void update(GLuint programID, GLuint mMatrixId, GLuint hasTextureId, GLuint hasMatId) {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		for (shared_ptr<ModelInstance> model : *models.get())
			model->update(programID, mMatrixId, hasTextureId, hasMatId);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

};

static shared_ptr<ModelBase> loadModel(shared_ptr<OBJ> obj) {

	shared_ptr<ModelBase> model{ make_shared<ModelBase>(obj->vertices->size(), obj->vertices->size(),obj->mtlConfig) };

	for (int i = 0; i < obj->vertices->size(); i++) {
		model->addVertex(obj->vertices->at(i), obj->normals->at(i), obj->texCoords->at(i));
		if (i % 3 == 0) model->addTriangle(i, i + 1, i + 2);
	}

	return model;
}

static shared_ptr<ModelBase> loadModel(shared_ptr<OBJ> obj, shared_ptr<BMPView> bmp) {

	shared_ptr<ModelBase> model{ make_shared<ModelBase>(obj->vertices->size(), obj->vertices->size(),bmp) };

	vec3 color{ 0.7,0.7,0.7 };
	for (int i = 0; i < obj->vertices->size(); i++) {
		//model->addVertex(obj->vertices->at(i), obj->normals->at(i), color);
		model->addVertex(obj->vertices->at(i), obj->normals->at(i), obj->texCoords->at(i));
		if (i % 3 == 0) model->addTriangle(i, i + 1, i + 2);
	}

	return model;
}

static shared_ptr<ModelBase> createIndexedModel(vector<vec3>& verticesIn, vector<vec3>& normalsIn) {
	vector<vec3> vertices{};
	vector<vec3> normals{};
	vector<int> indices{};

	for (int i = 0; i < verticesIn.size(); i++) {
		vec3 vertex{ verticesIn[i] };
		int index = 0;
		for (index = 0; index < vertices.size(); index++) {
			if (vertices[index] == vertex) break;
		}

		if (index == vertices.size()) {
			vertices.push_back(vertex);
			normals.push_back(normalsIn[i]);
		}
		else {
			normals[index] += normalsIn[i];
		}

		indices.push_back(index);
	}

	for (int i = 0; i < normals.size(); i++) {
		normals[i] = normalize(normals[i]);
	}

	return make_shared<IndexedObject>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec3>>(normals), make_shared<vector<int>>(indices));
}

static shared_ptr<ModelBase> loadSTLModel(string filename) {
	vector<vec3> vertices{};
	vector<vec3> normals{};

	loadSTLFile(vertices, normals, filename);

	return make_shared<VNObject>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec3>>(normals));
}

static shared_ptr<ModelBase> loadIndexedSTLModel(string fileName) {

	vector<vec3> vertices{};
	vector<vec3> normals{};

	loadSTLFile(vertices, normals, fileName);

	return createIndexedModel(vertices, normals);
}


#endif