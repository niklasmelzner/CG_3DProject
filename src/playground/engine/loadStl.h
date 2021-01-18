#ifndef LOAD_STL_H
#define LOAD_STL_H

#include "model.h"

namespace eng {

	class IndexedObject : public ModelBase {

	public:
		IndexedObject(shared_ptr<vector<vec3>> vertices, shared_ptr<vector<vec3>> normals, shared_ptr<vector<int>> indices, shared_ptr<MTLConfig> mtlConfig) :
			ModelBase(indices->size(), vertices->size(), mtlConfig) {
			vec3 color{ vec3(0.8,0.8,0.8) };
			for (int i = 0; i < vertices->size(); i++) addVertex(vertices->at(i), normals->at(i), vec2(0, 0));

			addIndices(indices);
		}

		inline void addIndices(shared_ptr<vector<int>> indices) {
			for (int i = 0; i < indices->size() / 3; i++) addTriangle(indices->at(i * 3), indices->at(i * 3 + 1), indices->at(i * 3 + 2));
		}
	};

	void loadSTLFile(vector< vec3 >& vertices, vector< vec3 >& normals, string stl_file_name) {
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

	class VNObject :public ModelBase {

	public:
		VNObject(shared_ptr<vector<vec3>> vertices, shared_ptr<vector<vec3>> normals, shared_ptr<MTLConfig> mtlConfig) :
			ModelBase(vertices->size(), vertices->size(), mtlConfig) {

			vec3 color{ vec3(0.8,0.8,0.8) };
			for (int i = 0; i < vertices->size(); i++) {
				addVertex(vec3(vertices->at(i)), vec3(normals->at(i)), vec2(0, 0));
				if (i % 3 == 0) addTriangle(i, i + 1, i + 2);
			}
		}

	};

	shared_ptr<ModelBase> loadSTLModel(string filename) {
		vector<vec3> vertices{};
		vector<vec3> normals{};

		loadSTLFile(vertices, normals, filename);

		return make_shared<VNObject>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec3>>(normals), defaultMTLConfig);
	}

	shared_ptr<ModelBase> createIndexedModel(vector<vec3>& verticesIn, vector<vec3>& normalsIn) {
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

		return make_shared<IndexedObject>(make_shared<vector<vec3>>(vertices), make_shared<vector<vec3>>(normals), make_shared<vector<int>>(indices), defaultMTLConfig);
	}

	shared_ptr<ModelBase> loadIndexedSTLModel(string fileName) {

		vector<vec3> vertices{};
		vector<vec3> normals{};

		loadSTLFile(vertices, normals, fileName);

		return createIndexedModel(vertices, normals);
	}

}

#endif