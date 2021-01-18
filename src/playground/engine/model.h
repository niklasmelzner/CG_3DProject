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
#include <chrono>
#include "light.h"

using namespace std;
using namespace glm;
using namespace std::chrono;

namespace eng {

	class AttrIds {

	public:
		GLuint programId;

		class LightIds {

		public:
			GLuint pos, color, type, modifiers, modifiers1;

			LightIds(GLuint programId, string lights, int i, string _pos, string _color, string _type, string _modifiers, string _modifiers1);

		};

		GLuint vertexArray, normalArray, texCoordArray, vp, v, m, mInv, viewPos, hasTexture, effectCount, lightCount;
		shared_ptr<vector<GLuint>> effectTypes{ make_shared<vector<GLuint>>() };
		shared_ptr<vector<GLuint>> effectValues{ make_shared<vector<GLuint>>() };
		shared_ptr<vector<shared_ptr<LightIds>>> lights{ make_shared<vector<shared_ptr<LightIds>>>() };

		AttrIds(int _programId, string _vertexArray, string _normalArray, string _texCoordArray,
			string _vp, string _v, string _m, string _mInv, string viewPos, string _hasTexture, string _effectCount,
			string _lightCount);

		void initEffectIds(string effects, string type, string value, int count);

		void initLightIds(string _lights, string type, string pos, string color, string modifiers, string modifiers1, int count);

	};

	class Mat4Supplier {
	public:
		virtual mat4 get() = 0;
	};

	class Transformation :public Mat4Supplier {
		shared_ptr<mat4> mat;
	public:

		Transformation() :mat(make_shared<mat4>(1.0f)) {};

		void reset();
		const void set(const mat4 _mat);

		inline mat4 get() override {
			return *mat;
		}

		inline mat4& operator*() {
			return *mat;
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

		mat4 get() override;

		inline void insert(const int index, const shared_ptr<Mat4Supplier> transformation) {
			transformations.insert((transformations.begin() + index), transformation);
		}

		shared_ptr<Transformation> newTransformation();

		shared_ptr<Transformation> newTransformation(int index);
	};

	class AnimationManager;

	class AnimatedObject :public enable_shared_from_this<AnimatedObject> {
		shared_ptr<AnimationManager> currentAM;
		int currentAMIndex;

	public:

		virtual void submitStep(long dt) = 0;

		inline int getAMId() {
			return currentAMIndex;
		}

		void removeFromAnimationManager();

		void setAnimationManager(shared_ptr<AnimationManager> aM, int index);

		void unsetAnimationManager(shared_ptr<AnimationManager> aM);

		~AnimatedObject();

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

		ModelBase(int _indexCount, int _vertexCount, shared_ptr<MTLConfig> _mtlConfig);

		inline float getMinDistanceToCenter() {
			calcMinMax();
			return minToCenter;
		}

		inline float getMaxDistanceToCenter() {
			calcMinMax();
			return maxToCenter;
		}

		inline shared_ptr<TransformationStack> getBaseTransformationStack() {
			return baseTransformationStack;
		}

		inline shared_ptr<Transformation> newBaseTransformation() {
			return baseTransformationStack->newTransformation();
		}

		inline vec3 getCenter() {
			calcDimensions();
			return pos + size / 2.0f;
		}

		inline vec3 getSize() {
			calcDimensions();
			return size;
		}

		void calcMinMax();

		float getAVGSize();

		void calcDimensions();

		void addVertex(vec3 v, vec3 n, vec2 t);

		void addTriangle(GLuint i0, GLuint i1, GLuint i2);

		void loadMTL(shared_ptr<MTL> mtl, GLuint programID, GLuint hasTextureId);

		void update(GLuint programID, shared_ptr<AttrIds> attrIds);

		~ModelBase();

	};

	class Effect {
	public:
		high_resolution_clock::time_point tBirth{ high_resolution_clock::now() };
		static const int LINEAR{ 0 }, STATIC{ 1 };
		float type, duration;
		vec3 value0, value1;
		int mode;

		Effect(float _type, vec3 _value0, vec3 _value1, int _mode, float _duration = 1) :
			type(_type), value0(_value0), value1(_value1), mode(_mode), duration(_duration) {
		}

		bool get(vec3& vec);

	};

	class ModelManager;

	class ModelInstance :public enable_shared_from_this<ModelInstance> {
		TransformationStack transformationStack{};
		shared_ptr<TransformationStack> transStackInsert{ make_shared<TransformationStack>() };
		shared_ptr<Transformation> transformation;
		shared_ptr<ModelManager> currentMM;
		int currentMMIndex{ 0 };
		vector<shared_ptr<Effect>> effects{};
	public:
		shared_ptr<ModelBase> model;
		shared_ptr<vector<shared_ptr<LightSource>>> lightSources{ make_shared<vector<shared_ptr<LightSource>>>() };

		ModelInstance(shared_ptr<ModelBase> _model);

		inline mat4 getTransformEnd() {
			return transformationStack.get();
		}

		inline mat4 getTransformInstance() {
			return transformation->get() * transStackInsert->get();
		}

		inline int getMMId() {
			return currentMMIndex;
		}

		void addLight(shared_ptr<LightSource> lightSource);

		inline void addEffect(shared_ptr<Effect> effect) {
			effects.push_back(effect);
		}

		inline void removeEffect(shared_ptr<Effect> _effect) {
			effects.erase(remove_if(effects.begin(), effects.end(), [=](shared_ptr<Effect>effect) {
				return _effect.get() == effect.get();
				}), effects.end());
		}

		inline shared_ptr<Transformation> getModelTransformation() {
			return transformation;
		}

		inline shared_ptr<Transformation> newModelTransformation() {
			return transStackInsert->newTransformation();
		}

		inline shared_ptr<Transformation> newModelTransformation(int index) {
			return transStackInsert->newTransformation(index);
		}

		void unsetModelManager(shared_ptr<ModelManager> modelManager);

		void setModelManager(shared_ptr<ModelManager> modelManager, int index);

		void update(GLuint programID, shared_ptr<AttrIds> attrIds);

		void removeFromModelManager();

		~ModelInstance();
	};

	class AnimationManager : public enable_shared_from_this<AnimationManager> {

		vector<shared_ptr<AnimatedObject>> animatedObjects{};

	public:

		void add(shared_ptr<AnimatedObject> object);

		void remove(shared_ptr<AnimatedObject> ptr);

		void update(long dt);

		~AnimationManager();

	};

	class ModelManager : public enable_shared_from_this<ModelManager> {
		vector<shared_ptr<ModelInstance>> models{};

	public:
		vector<shared_ptr<LightSource>> lightSources{};

		inline void addLight(shared_ptr<LightSource> lightSource) {
			lightSources.push_back(lightSource);
		}

		inline void add(shared_ptr<ModelInstance> model) {
			model->setModelManager(shared_from_this(), models.size());
			models.push_back(model);
		}

		inline void remove(shared_ptr<ModelInstance> ptr) {
			models.erase(remove_if(models.begin(), models.end(), [=](shared_ptr<ModelInstance> m) {
				return m.get() == ptr.get();
				}), models.end());
		}

		void removeLight(shared_ptr<LightSource> _lightSource);

		void addLights(shared_ptr<vector<shared_ptr<LightSource>>> _lightSources);

		void removeLights(shared_ptr<vector<shared_ptr<LightSource>>> _lightSources);

		void update(GLuint programID, shared_ptr<AttrIds> attrIds);

		~ModelManager();

	};

	shared_ptr<ModelBase> loadModel(shared_ptr<OBJ> obj);
}

#endif