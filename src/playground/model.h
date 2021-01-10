#ifndef MODEL_H
#define MODEL_H

#include "modellOADER.h";

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include "parse_stl.h"
#include <fstream>
#include <regex>

using namespace std;
using namespace glm;

struct TMat4 : public mat4 {

	template<typename... Types>
	TMat4(Types... types) :mat4(types...) {}

	const void set(const mat4 mat) {
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++)
				(*this)[x][y] = mat[x][y];
	}
};

class Mat4Supplier {
public:
	virtual mat4 get() = 0;
};

class Transformation :public Mat4Supplier {
	shared_ptr<mat4> mat;
public:

	Transformation() :mat(make_shared<mat4>(1.0f)) {};

	mat4 get() override {
		return *mat;
	}

	mat4& operator*() {
		return *mat;
	}

	void reset() {
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				(*mat)[y][x] = x == y ? 1 : 0;
	}

	const void set(const mat4 _mat) {
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				(*mat)[y][x] = _mat[y][x];
	}

	inline void translate(const vec3 vec) {
		translate(vec.x, vec.y, vec.z);
	}

	inline void translate(const float x, const float y, const float z) {
		transform(glm::translate(vec3(x, y, z)));
	}

	inline void scale(const float sxyz) {
		scale(sxyz, sxyz, sxyz);
	}

	inline void scale(const float sx, const float sy, const float sz) {
		transform(glm::scale(vec3(sx, sy, sz)));
	}

	inline void transform(const mat4 _mat) {
		set(_mat * *mat);
	}

	inline void rotate(float angle, vec3 axis) {
		transform(glm::rotate(angle, axis));
	}

	inline void setTranslation(const vec3 vec) {
		(*mat)[3][0] = vec.x;
		(*mat)[3][1] = vec.y;
		(*mat)[3][2] = vec.z;
	}

	inline void setScale(const float sxyz) {
		setScale(sxyz, sxyz, sxyz);
	}

	inline void setScale(const float sx, const float sy, const float sz) {
		(*mat)[0][0] = sx;
		(*mat)[1][1] = sy;
		(*mat)[2][2] = sz;
	}

};

class TransformationStack : public Mat4Supplier {
	vector<shared_ptr<Mat4Supplier>> transformations{};
public:

	mat4 get() override {
		if (transformations.size() == 0)return mat4(1.0f);
		mat4 mat = transformations[0]->get();

		for (int i = 1; i < transformations.size(); i++)mat = transformations[i]->get() * mat;

		return mat;
	}

	void insert(const int index, const shared_ptr<Mat4Supplier> transformation) {
		transformations.insert((transformations.begin() + index), transformation);
	}

	shared_ptr<Transformation> newTransformation() {
		shared_ptr<Transformation> transformation{ make_shared<Transformation>() };
		transformations.push_back(transformation);
		return transformation;
	}

	shared_ptr<Transformation> newTransformation(int index) {
		shared_ptr<Transformation> transformation{ make_shared<Transformation>() };
		insert(index, transformation);
		return transformation;
	}
};


template<class T>
class Manager {
public:
	virtual void remove(shared_ptr<T>) = 0;

};

class AnimatedObject :public enable_shared_from_this<AnimatedObject> {
	shared_ptr<Manager<AnimatedObject>> currentAM;
	int currentAMIndex;

public:

	int getAMId() {
		return currentAMIndex;
	}

	void removeFromAnimationManager() {
		if (currentAM != nullptr) {
			currentAM->remove(shared_from_this());
			currentAM = nullptr;
		}
	}

	void setAnimationManager(shared_ptr<Manager<AnimatedObject>> aM, int index) {
		if (currentAM != nullptr && currentAM.get() != aM.get())
			currentAM->remove(shared_from_this());

		currentAM = aM;
		currentAMIndex = index;
	}

	virtual void precalculateStep(long dt) = 0;

	virtual void submitStep(long dt) = 0;

	void unsetAnimationManager(shared_ptr<Manager<AnimatedObject>> aM) {
		if (currentAM.get() == aM.get())
			currentAM = nullptr;

	}

	~AnimatedObject() {
		removeFromAnimationManager();
	}

};

class ModelBase {
	shared_ptr<GLfloat> vertex_buffer_data;
	shared_ptr<GLfloat> normal_buffer_data;
	shared_ptr<GLuint> index_buffer_data;
	shared_ptr<GLfloat> texture_buffer_data;
	shared_ptr<MTLConfig> mtlConfig;
	const int vertexCount, indexCount;
	int vertexInsertIndex{ 0 }, normalInsertIndex{ 0 }, indexInsertIndex{ 0 };
	GLuint vertexbuffer, normalbuffer, indexbuffer, texturebuffer;
	bool vertexDataChanged{ true }, indexDataChanged{ false };
	shared_ptr<TransformationStack> baseTransformationStack{ make_shared<TransformationStack>() };
public:
	bool calcedPosNSize{ 0 }, calcedMinMax{ 0 };
	vec3 size, pos;
	float minToCenter, maxToCenter;

	ModelBase(int _indexCount, int _vertexCount, shared_ptr<MTLConfig> _mtlConfig) :
		indexCount(_indexCount), vertexCount(_vertexCount), mtlConfig(_mtlConfig) {

		vertex_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &vertexbuffer);

		normal_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 3],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &normalbuffer);

		index_buffer_data = std::shared_ptr<GLuint>{ new GLuint[indexCount],std::default_delete<GLuint[]>() };
		glGenBuffers(1, &indexbuffer);

		texture_buffer_data = std::shared_ptr<GLfloat>{ new GLfloat[vertexCount * 2],std::default_delete<GLfloat[]>() };
		glGenBuffers(1, &texturebuffer);
	}

	float getMinDistanceToCenter() {
		calcMinMax();
		return minToCenter;
	}

	void calcMinMax() {
		if (!calcedMinMax) {
			calcedMinMax = true;

			vec3 center{ getCenter() };

			float dMin = 0, dMax = 0;
			for (int i = 0; i < vertexCount; i++) {
				vec3 v{
					vertex_buffer_data.get()[i * 3],
					vertex_buffer_data.get()[i * 3 + 1],
					vertex_buffer_data.get()[i * 3 + 2]
				};
				float d{ length(v - center) };
				if (i == 0 || d < dMin) dMin = d;
				if (i == 0 || d > dMax) dMax = d;
			}
			minToCenter = dMin;
			maxToCenter = dMax;
		}
	}

	float getMaxDistanceToCenter() {
		calcMinMax();
		return maxToCenter;
	}

	float getAVGSize() {
		mat4 mat{ baseTransformationStack->get() };
		mat[3][0] = 0;
		mat[3][1] = 0;
		mat[3][2] = 0;
		calcMinMax();
		return length(vec3((minToCenter + maxToCenter) * mat * vec4(1, 0, 0, 1)));
	}

	shared_ptr<TransformationStack> getBaseTransformationStack() {
		return baseTransformationStack;
	}

	shared_ptr<Transformation> newBaseTransformation() {
		return baseTransformationStack->newTransformation();
	}

	void calcDimensions() {
		assert(vertexInsertIndex > 0);

		if (!calcedPosNSize) {
			calcedPosNSize = true;
			GLfloat xMin{ vertex_buffer_data.get()[0] },
				xMax{ xMin },
				yMin{ vertex_buffer_data.get()[1] },
				yMax{ yMin },
				zMin{ vertex_buffer_data.get()[2] },
				zMax{ zMin };

			for (int i = 1; i < vertexInsertIndex; i++) {
				GLfloat x{ vertex_buffer_data.get()[i * 3] };
				GLfloat y{ vertex_buffer_data.get()[i * 3 + 1] };
				GLfloat z{ vertex_buffer_data.get()[i * 3 + 2] };

				if (x < xMin) xMin = x;
				else if (x > xMax) xMax = x;
				if (y < yMin) yMin = y;
				else if (y > yMax) yMax = y;
				if (z < zMin) zMin = z;
				else if (z > zMax) zMax = z;
			}

			pos = { xMin,yMin,zMin };
			size = { xMax - xMin,yMax - yMin,zMax - zMin };
		}
	}

	vec3 getCenter() {
		calcDimensions();
		return pos + size / 2.0f;
	}

	vec3 getSize() {
		calcDimensions();
		return size;
	}

	void addVertex(vec3 v, vec3 n, vec2 t) {
		vertex_buffer_data.get()[vertexInsertIndex * 3] = v.x;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 1] = v.y;
		vertex_buffer_data.get()[vertexInsertIndex * 3 + 2] = v.z;
		normal_buffer_data.get()[vertexInsertIndex * 3] = n.x;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 1] = n.y;
		normal_buffer_data.get()[vertexInsertIndex * 3 + 2] = n.z;
		texture_buffer_data.get()[vertexInsertIndex * 2] = t.x;
		texture_buffer_data.get()[vertexInsertIndex * 2 + 1] = t.y;
		vertexInsertIndex++;
		calcedPosNSize = calcedMinMax = false;
		vertexDataChanged = true;
	}

	void addTriangle(GLuint i0, GLuint i1, GLuint i2) {
		index_buffer_data.get()[indexInsertIndex++] = i0;
		index_buffer_data.get()[indexInsertIndex++] = i1;
		index_buffer_data.get()[indexInsertIndex++] = i2;
		indexDataChanged = true;
	}

	void loadMTL(shared_ptr<MTL> mtl, GLuint programID, GLuint hasTextureId) {
		bool hasTexture = mtl->mapDiffuse.get() != nullptr;
		if (hasTexture) {
			glBindTexture(GL_TEXTURE_2D, mtl->mapDiffuse->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glUniform1f(hasTextureId, 1.0);
		}
		else glUniform1f(hasTextureId, 0.0);
		glUniform3f(glGetUniformLocation(programID, "material.ambient"), mtl->ambient.r, mtl->ambient.g, mtl->ambient.b);
		glUniform3f(glGetUniformLocation(programID, "material.diffuse"), mtl->diffuse.r, mtl->diffuse.g, mtl->diffuse.b);
		glUniform3f(glGetUniformLocation(programID, "material.specular"), mtl->specular.r, mtl->specular.g, mtl->specular.b);
	}

	void update(GLuint programID, GLuint hasTextureId) {
		if (vertexDataChanged) {
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 3, vertex_buffer_data.get(), GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 3, normal_buffer_data.get(), GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 2, texture_buffer_data.get(), GL_STATIC_DRAW);

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

		vector<int>* indices{ &mtlConfig->indices };
		for (int i = 0, size = indices->size(); i < size; i++) {
			shared_ptr<MTL> mtl{ mtlConfig->get(indices->at(i)) };
			loadMTL(mtl, programID, hasTextureId);
			bool textureActive = mtl != nullptr;

			if (textureActive)glEnableVertexAttribArray(2);

			int start{ indices->at(i) };
			int end{ i == size - 1 ? indexCount : indices->at(i + 1) };
			glDrawElementsBaseVertex(GL_TRIANGLES, (end - start), GL_UNSIGNED_INT, 0, start);
			if (textureActive)glDisableVertexAttribArray(2);
		}

	}

	~ModelBase() {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &indexbuffer);
		glDeleteBuffers(1, &normalbuffer);
	}

};

class ModelInstance :public enable_shared_from_this<ModelInstance> {
	TransformationStack transformationStack{};
	shared_ptr<TransformationStack> transStackInsert{ make_shared<TransformationStack>() };
	shared_ptr<Transformation> transformation;
	shared_ptr<Manager<ModelInstance>> currentMM;
	int currentMMIndex;

public:
	shared_ptr<ModelBase> model;

	int getMMId() {
		return currentMMIndex;
	}

	ModelInstance(shared_ptr<ModelBase> _model) :model(_model), transformation(transformationStack.newTransformation()) {
		transformationStack.insert(0, _model->getBaseTransformationStack());
		transformationStack.insert(1, transStackInsert);
	}

	void unsetModelManager(shared_ptr<Manager<ModelInstance>> modelManager) {
		if (currentMM.get() == modelManager.get()) currentMM = nullptr;
	}

	void setModelManager(shared_ptr<Manager<ModelInstance>> modelManager, int index) {
		if (currentMM.get() != nullptr && currentMM.get() != modelManager.get())
			currentMM->remove(shared_from_this());

		currentMM = modelManager;
		currentMMIndex = index;
	}

	void update(GLuint programID, GLuint mMatrixId, GLuint hasTextureId, GLuint mInvMatrixId) {
		mat4 m{ transformationStack.get() };
		glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &(m)[0][0]);
		glUniformMatrix4fv(mInvMatrixId, 1, GL_FALSE, &(glm::transpose(glm::inverse(m)))[0][0]);
		;
		model->update(programID, hasTextureId);
	}

	inline shared_ptr<Transformation> getModelTransformation() {
		return transformation;
	}

	shared_ptr<Transformation> newModelTransformation() {
		return transStackInsert->newTransformation();
	}

	shared_ptr<Transformation> newModelTransformation(int index) {
		return transStackInsert->newTransformation(index);
	}

	void removeFromModelManager() {
		if (currentMM != nullptr) {
			currentMM->remove(shared_from_this());
			currentMM = nullptr;
		};
	}

	~ModelInstance() {
		removeFromModelManager();
	}
};

class AnimationManager :public Manager<AnimatedObject>, public enable_shared_from_this<AnimationManager> {

	vector<shared_ptr<AnimatedObject>> animatedObjects{};

public:

	void add(shared_ptr<AnimatedObject> object) {
		animatedObjects.push_back(object);
		object->setAnimationManager(shared_from_this(), animatedObjects.size());
	}

	void remove(shared_ptr<AnimatedObject> ptr) override {
		animatedObjects.erase(remove_if(animatedObjects.begin(), animatedObjects.end(), [=](shared_ptr<AnimatedObject> m) {
			return m.get() == ptr.get();
			}), animatedObjects.end());
	}

	void update(long dt) {
		for (shared_ptr<AnimatedObject> model : animatedObjects) model->precalculateStep(dt);
		for (shared_ptr<AnimatedObject> model : animatedObjects) model->submitStep(dt);
	}

	~AnimationManager() {
		for (shared_ptr<AnimatedObject> model : animatedObjects)
			model->unsetAnimationManager(shared_from_this());
	}

};

class ModelManager :public Manager<ModelInstance>, public enable_shared_from_this<ModelManager> {
	vector<shared_ptr<ModelInstance>> models{};

public:

	void add(shared_ptr<ModelInstance> model) {
		model->setModelManager(shared_from_this(), models.size());
		models.push_back(model);
	}

	void remove(shared_ptr<ModelInstance> ptr) override {
		models.erase(remove_if(models.begin(), models.end(), [=](shared_ptr<ModelInstance> m) {
			return m.get() == ptr.get();
			}), models.end());
	}

	void update(GLuint programID, GLuint mMatrixId, GLuint hasTextureId, GLuint mInvMatrixId) {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		for (shared_ptr<ModelInstance> model : models)
			model->update(programID, mMatrixId, hasTextureId, mInvMatrixId);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	~ModelManager() {
		for (shared_ptr<ModelInstance> model : models)
			model->unsetModelManager(shared_from_this());
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

#endif