#include "model.h";

namespace eng {

	AttrIds::LightIds::LightIds(GLuint programId, string lights, int i, string _pos, string _color, string _type, string _modifiers, string _modifiers1) {
		{
			pos = glGetUniformLocation(programId, (lights + "[" + to_string(i) + "]." + _pos).c_str());
			color = glGetUniformLocation(programId, (lights + "[" + to_string(i) + "]." + _color).c_str());
			type = glGetUniformLocation(programId, (lights + "[" + to_string(i) + "]." + _type).c_str());
			modifiers = glGetUniformLocation(programId, (lights + "[" + to_string(i) + "]." + _modifiers).c_str());
			modifiers1 = glGetUniformLocation(programId, (lights + "[" + to_string(i) + "]." + _modifiers1).c_str());
		}
	}

	AttrIds::AttrIds(int _programId, string _vertexArray, string _normalArray, string _texCoordArray,
		string _vp, string _v, string _m, string _mInv, string _viewPos, string _hasTexture, string _effectCount,
		string _lightCount) :programId(_programId) {

		vertexArray = glGetAttribLocation(programId, _vertexArray.c_str());
		normalArray = glGetAttribLocation(programId, _normalArray.c_str());
		texCoordArray = glGetAttribLocation(programId, _texCoordArray.c_str());

		vp = glGetUniformLocation(programId, _vp.c_str());
		v = glGetUniformLocation(programId, _v.c_str());
		m = glGetUniformLocation(programId, _m.c_str());
		mInv = glGetUniformLocation(programId, _mInv.c_str());
		viewPos = glGetUniformLocation(programId, _viewPos.c_str());
		hasTexture = glGetUniformLocation(programId, _hasTexture.c_str());
		effectCount = glGetUniformLocation(programId, _effectCount.c_str());
		lightCount = glGetUniformLocation(programId, _lightCount.c_str());
	}

	void AttrIds::initEffectIds(string effects, string type, string value, int count) {
		for (int i = 0; i < count; i++) {
			effectTypes->push_back(glGetUniformLocation(programId, (effects + "[" + to_string(i) + "]." + type).c_str()));
			effectValues->push_back(glGetUniformLocation(programId, (effects + "[" + to_string(i) + "]." + value).c_str()));
		}
	}

	void AttrIds::initLightIds(string _lights, string type, string pos, string color, string modifiers, string modifiers1, int count) {
		for (int i = 0; i < count; i++)
			lights->push_back(make_shared<LightIds>(programId, _lights, i, pos, color, type, modifiers, modifiers1));
	}



	void Transformation::reset() {
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				(*mat)[y][x] = x == y ? 1 : 0;
	}

	const void Transformation::set(const mat4 _mat) {
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				(*mat)[y][x] = _mat[y][x];
	}

	mat4 TransformationStack::get() {
		if (transformations.size() == 0)return mat4(1.0f);
		mat4 mat = transformations[0]->get();

		for (int i = 1; i < transformations.size(); i++)mat = transformations[i]->get() * mat;

		return mat;
	}

	shared_ptr<Transformation> TransformationStack::newTransformation() {
		shared_ptr<Transformation> transformation{ make_shared<Transformation>() };
		transformations.push_back(transformation);
		return transformation;
	}

	shared_ptr<Transformation> TransformationStack::newTransformation(int index) {
		shared_ptr<Transformation> transformation{ make_shared<Transformation>() };
		insert(index, transformation);
		return transformation;
	}


	void AnimatedObject::removeFromAnimationManager() {
		if (currentAM != nullptr) {
			currentAM->remove(shared_from_this());
			currentAM = nullptr;
		}
	}

	void AnimatedObject::setAnimationManager(shared_ptr<AnimationManager> aM, int index) {
		if (currentAM != nullptr && currentAM.get() != aM.get())
			currentAM->remove(shared_from_this());

		currentAM = aM;
		currentAMIndex = index;
	}

	void AnimatedObject::unsetAnimationManager(shared_ptr<AnimationManager> aM) {
		if (currentAM.get() == aM.get())
			currentAM = nullptr;
	}

	AnimatedObject::~AnimatedObject() {
		removeFromAnimationManager();
	}

	ModelBase::ModelBase(int _indexCount, int _vertexCount, shared_ptr<MTLConfig> _mtlConfig) :
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

	void ModelBase::calcMinMax() {
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

	float ModelBase::getAVGSize() {
		mat4 mat{ baseTransformationStack->get() };
		mat[3][0] = 0;
		mat[3][1] = 0;
		mat[3][2] = 0;
		calcMinMax();
		//return length(vec3((1.3f * maxToCenter + 0.7f * minToCenter) * mat * vec4(1, 0, 0, 1)));
		return length(vec3(0.82f * maxToCenter * 2 * mat * vec4(1, 0, 0, 1)));
	}

	void ModelBase::calcDimensions() {
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

	void ModelBase::addVertex(vec3 v, vec3 n, vec2 t) {
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

	void ModelBase::addTriangle(GLuint i0, GLuint i1, GLuint i2) {
		index_buffer_data.get()[indexInsertIndex++] = i0;
		index_buffer_data.get()[indexInsertIndex++] = i1;
		index_buffer_data.get()[indexInsertIndex++] = i2;
		indexDataChanged = true;
	}

	void ModelBase::loadMTL(shared_ptr<MTL> mtl, GLuint programID, GLuint hasTextureId,GLuint texArrayId) {
		bool hasTexture = mtl->mapDiffuse.get() != nullptr;
		if (hasTexture) {
			glBindTexture(GL_TEXTURE_2D, mtl->mapDiffuse->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
			glVertexAttribPointer(texArrayId, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glUniform1f(hasTextureId, 1.0);
		}
		else glUniform1f(hasTextureId, 0.0);
		glUniform3f(glGetUniformLocation(programID, "material.ambient"), mtl->ambient.r, mtl->ambient.g, mtl->ambient.b);
		glUniform3f(glGetUniformLocation(programID, "material.diffuse"), mtl->diffuse.r, mtl->diffuse.g, mtl->diffuse.b);
		glUniform3f(glGetUniformLocation(programID, "material.specular"), mtl->specular.r, mtl->specular.g, mtl->specular.b);
	}

	void ModelBase::update(GLuint programID, shared_ptr<AttrIds> attrIds) {
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
		glVertexAttribPointer(attrIds->vertexArray, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(attrIds->normalArray, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		vector<int>* indices{ &mtlConfig->indices };
		for (int i = 0, size = indices->size(); i < size; i++) {
			shared_ptr<MTL> mtl{ mtlConfig->get(indices->at(i)) };
			loadMTL(mtl, programID, attrIds->hasTexture,attrIds->texCoordArray);
			bool textureActive = mtl != nullptr;

			if (textureActive) glEnableVertexAttribArray(attrIds->texCoordArray);

			int start{ indices->at(i) };
			int end{ i == size - 1 ? indexCount : indices->at(i + 1) };
			glDrawElementsBaseVertex(GL_TRIANGLES, (end - start), GL_UNSIGNED_INT, 0, start);
			if (textureActive) glDisableVertexAttribArray(attrIds->texCoordArray);
		}

	}

	ModelBase::~ModelBase() {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &indexbuffer);
		glDeleteBuffers(1, &normalbuffer);
	}

	bool Effect::get(vec3& vec) {
		if (type == 0) {
			if (mode == LINEAR) {
				float prg{ std::min(1.0f,duration_cast<milliseconds>(high_resolution_clock::now() - tBirth).count() / duration) };

				vec = value0 + (value1 - value0) * prg;

				return prg == 1.0;
			}
			else if (mode == STATIC) {
				vec = value0;
				return 0;
			}
		}

		return 1;
	}

	ModelInstance::ModelInstance(shared_ptr<ModelBase> _model) :model(_model), transformation(transformationStack.newTransformation()) {
		transformationStack.insert(0, _model->getBaseTransformationStack());
		transformationStack.insert(1, transStackInsert);
	}

	void ModelInstance::addLight(shared_ptr<LightSource> lightSource) {
		lightSources->push_back(lightSource);
		if (currentMM != nullptr) currentMM->addLight(lightSource);
	}

	void ModelInstance::unsetModelManager(shared_ptr<ModelManager> modelManager) {
		if (currentMM.get() == modelManager.get()) {
			currentMM->removeLights(lightSources);
			currentMM = nullptr;
		}
	}

	void ModelInstance::setModelManager(shared_ptr<ModelManager> modelManager, int index) {
		if (currentMM != nullptr && currentMM.get() != modelManager.get()) {
			currentMM->removeLights(lightSources);
			currentMM->remove(shared_from_this());
		}

		currentMM = modelManager;
		modelManager->addLights(lightSources);
		currentMMIndex = index;
	}

	void ModelInstance::update(GLuint programID, shared_ptr<AttrIds> attrIds) {
		mat4 m{ transformationStack.get() };
		glUniformMatrix4fv(attrIds->m, 1, GL_FALSE, &(m)[0][0]);
		//glUniformMatrix4fv(attrIds->mInv, 1, GL_FALSE, &(glm::transpose(glm::inverse(m)))[0][0]);

		glUniform1i(attrIds->effectCount, effects.size());
		for (int i = 0; i < effects.size(); i++) {
			glUniform1f(attrIds->effectTypes->at(i), effects[i]->type);
			vec3 v{};
			bool rem{ effects[i]->get(v) };
			glUniform3f(attrIds->effectValues->at(i), v.x, v.y, v.z);
			if (rem) effects.erase(effects.begin() + i--);
		}

		model->update(programID, attrIds);
	}

	void ModelInstance::removeFromModelManager() {
		if (currentMM != nullptr) {
			currentMM->remove(shared_from_this());
			currentMM->removeLights(lightSources);
			currentMM = nullptr;
		};
	}

	ModelInstance::~ModelInstance() {
		removeFromModelManager();
	}

	void AnimationManager::add(shared_ptr<AnimatedObject> object) {
		animatedObjects.push_back(object);
		object->setAnimationManager(shared_from_this(), animatedObjects.size());
	}

	void AnimationManager::remove(shared_ptr<AnimatedObject> ptr) {
		animatedObjects.erase(remove_if(animatedObjects.begin(), animatedObjects.end(), [=](shared_ptr<AnimatedObject> m) {
			return m.get() == ptr.get();
			}), animatedObjects.end());
	}

	void AnimationManager::update(long dt) {
		for (shared_ptr<AnimatedObject> model : animatedObjects) model->submitStep(dt);
	}

	AnimationManager::~AnimationManager() {
		for (shared_ptr<AnimatedObject> model : animatedObjects)
			model->unsetAnimationManager(shared_from_this());
	}

	void ModelManager::removeLight(shared_ptr<LightSource> _lightSource) {
		lightSources.erase(remove_if(lightSources.begin(), lightSources.end(), [=](shared_ptr<LightSource> lightSource) {
			return lightSource.get() == _lightSource.get();
			}), lightSources.end());
	}

	void ModelManager::addLights(shared_ptr<vector<shared_ptr<LightSource>>> _lightSources) {
		for (shared_ptr<LightSource> lightSource : *_lightSources)
			lightSources.push_back(lightSource);
	}

	void ModelManager::removeLights(shared_ptr<vector<shared_ptr<LightSource>>> _lightSources) {
		lightSources.erase(remove_if(lightSources.begin(), lightSources.end(), [=](shared_ptr<LightSource> lightSource) {
			for (shared_ptr<LightSource> _lightSource : *_lightSources) if (_lightSource.get() == lightSource.get()) return true;
			return false;
			}), lightSources.end());
	}

	void ModelManager::update(GLuint programID, shared_ptr<AttrIds> attrIds) {
		glUniform1i(attrIds->lightCount, lightSources.size());

		for (int i = 0; i < lightSources.size(); i++) {
			shared_ptr<LightSource> lightSource{ lightSources[i] };
			lightSource->updateTransform();
			glUniform1i(attrIds->lights->at(i)->type, lightSource->type);
			glUniform3f(attrIds->lights->at(i)->pos, lightSource->pos.x, lightSource->pos.y, lightSource->pos.z);
			glUniform3f(attrIds->lights->at(i)->color, lightSource->color.r, lightSource->color.y, lightSource->color.z);
			glUniform4f(attrIds->lights->at(i)->modifiers, lightSource->modifiers.x, lightSource->modifiers.y, lightSource->modifiers.z, lightSource->modifiers.w);
			glUniform4f(attrIds->lights->at(i)->modifiers1, lightSource->modifiers1.x, lightSource->modifiers1.y, lightSource->modifiers1.z, lightSource->modifiers1.w);
		}

		glEnableVertexAttribArray(attrIds->vertexArray);
		glEnableVertexAttribArray(attrIds->normalArray);
		for (shared_ptr<ModelInstance> model : models)
			model->update(programID, attrIds);
		glDisableVertexAttribArray(attrIds->vertexArray);
		glDisableVertexAttribArray(attrIds->normalArray);
	}

	ModelManager::~ModelManager() {
		for (shared_ptr<ModelInstance> model : models)
			model->unsetModelManager(shared_from_this());
	}

	shared_ptr<ModelBase> loadModel(shared_ptr<OBJ> obj) {

		shared_ptr<ModelBase> model{ make_shared<ModelBase>(obj->vertices->size(), obj->vertices->size(),obj->mtlConfig) };

		for (int i = 0; i < obj->vertices->size(); i++) {
			model->addVertex(obj->vertices->at(i), obj->normals->at(i), obj->texCoords->at(i));
			if (i % 3 == 0) model->addTriangle(i, i + 1, i + 2);
		}

		return model;
	}

}
