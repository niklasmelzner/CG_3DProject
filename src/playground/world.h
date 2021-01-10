#ifndef WORLD_H
#define WORLD_H

#include "model.h"
#include <map>
#include <functional>

static double PI = 3.141592653589793238462643383279;

class Cuboid {
public:
	vec3 p0, p1, size;
	Cuboid() {}
	Cuboid(vec3 _pos, vec3 _size) :p0(_pos), p1(_pos + _size), size(_size) {}

	bool contains(vec3 _pos) {
		return p0.x <= _pos.x && _pos.x < p1.x&& p0.y <= _pos.y && _pos.y < p1.y&& p0.z <= _pos.z && _pos.z < p1.z;
	}

	void print() {
		print(p0);
		std::cout << " | ";
		print(p1);
	}

	void print(vec3 pos) {
		std::cout << pos.x << ", " << pos.y << ", " << pos.y;
	}

	bool operator !=(Cuboid other) {
		return p0 != other.p0 || size != other.size;
	}

};

class World;

class WorldObject :public AnimatedObject {
	vec3 location;
	vec3 speed{}, rotationAxis{ vec3(1,0,0) };
	shared_ptr<ModelInstance> model;
	shared_ptr<World> world;
	bool deletable{ true };
	float size, mass, rotationSpeed{ 0 }, density{ 1 };
	bool calcMass;
	bool collisionListenerSet{ false };
	function<int(int, shared_ptr<WorldObject>)> onCollisionListener;
public:
	const int type;

	WorldObject(int type, shared_ptr<ModelInstance> model, vec3 pos, float size) :WorldObject(type, model, pos, size, PI* size* size* size / 6) {
		calcMass = true;
	}

	WorldObject(int _type, shared_ptr<ModelInstance> _model, vec3 _pos, float _size, float _mass) :AnimatedObject(),
		model(_model), location(_pos), size(_size), mass(_mass), type(_type) {
		calcMass = false;
	}

	WorldObject(int type, float _size) :WorldObject(type, nullptr, vec3{}, size) {}

	bool hasCollisionListener() {
		return collisionListenerSet;
	}

	void setDensity(float _density) {
		mass *= _density / density;
		density = _density;
	}

	int execOnCollision(int type, shared_ptr<WorldObject> obj) {
		return onCollisionListener(type, obj);
	}

	void onCollision(function<int(int type, shared_ptr<WorldObject>)> listener) {
		onCollisionListener = listener;
		collisionListenerSet = true;
	}

	void setSize(float _size) {
		size = _size;
		if (calcMass) mass = PI * size * size * size / 6 * density;
	}

	float getSize() {
		return size;
	}

	void setSize(shared_ptr<ModelInstance> model) {
		setSize(model->model->getAVGSize());
	}

	void setDeletable(bool _deletable) { deletable = _deletable; }

	bool isDeletable() { return deletable; };

	void die() {
		removeFromAnimationManager();
		model->removeFromModelManager();
	}

	void init(shared_ptr<World> _world) { world = _world; }

	void setModelInstance(shared_ptr<ModelInstance> _model) {
		if (model != nullptr) {
			std::cout << "model instance has already been set\n";
			throw 1;
		}
		model = _model;
	}

	shared_ptr<ModelInstance> getModel() { return model; }

	void setSpeed(vec3 _speed) { speed = _speed; }

	vec3 getSpeed() {
		return speed;
	}

	float getMass() {
		return mass;
	}

	void setRotationSpeed(float speed, vec3 axis) {
		rotationSpeed = speed;
		rotationAxis = axis;
	}

	void precalculateStep(long dt) override {}

	vec3 getLocation() { return location; }

	vec3 setLocation(vec3 _location) { location = _location; }

	void submitStep(long dt) {
		float f{ dt / 1000.0f };
		update(f);

		location += speed * f;
		model->getModelTransformation()->rotate(f * rotationSpeed, rotationAxis);
		model->getModelTransformation()->setTranslation(location);
	}

	virtual void update(float f) {};

};

class World :public enable_shared_from_this<World> {
	vector<shared_ptr<WorldObject>> objects{};
	const vec3 size, offset;
	vec3 center{};
	Cuboid bounds;
	const shared_ptr<ModelManager> modelManager{ make_shared<ModelManager>() };
	const shared_ptr<AnimationManager> animationManager{ make_shared<AnimationManager>() };
	map<int, vector<std::function<void(const shared_ptr<const WorldObject>)>>> onDeleteListeners;
	vector < std::function<void(const Cuboid&, const  Cuboid&)>> onUpdateListeners{};

	void execOnRemove(shared_ptr<WorldObject> obj) {
		if (onDeleteListeners.count(obj->type)) {
			for (function<void(shared_ptr<WorldObject>)> listener : onDeleteListeners[obj->type]) {
				listener(obj);
			}
		}
		obj->die();
	}
public:
	static const int COLL_TYPE_BOUNCE_OFF{ 1 }, COLL_TYPE_DELETE_OTHER{ 2 }, COLL_TYPE_DEFAULT{ COLL_TYPE_BOUNCE_OFF };

	World(vec3 _size, vec3 _offset) :
		size(_size), offset(_offset - _size / 2.0f),
		bounds(createBoundsArround(center)) {
	}

	void onDelete(int type, std::function<void(const shared_ptr<const WorldObject>)> listener) {
		onDeleteListeners[type].push_back(listener);
	}

	void onUpdate(std::function<void(const Cuboid&, const Cuboid&)> listener) {
		onUpdateListeners.push_back(listener);
	}

	template<typename... Types>
	void updateAM(Types... args) { animationManager->update(args...); }

	Cuboid createBoundsArround(vec3 pos) { return Cuboid{ pos + offset,size }; }

	void deleteOutOfBounds(Cuboid bounds) {
		objects.erase(remove_if(objects.begin(), objects.end(), [&](shared_ptr<WorldObject> obj) {
			bool rem{ obj->isDeletable() && !bounds.contains(obj->getLocation()) };
			if (rem) execOnRemove(obj);
			return rem;
			}), objects.end());
	}

	void remove(shared_ptr<WorldObject> obj) {
		objects.erase(remove_if(objects.begin(), objects.end(), [&](shared_ptr<WorldObject> obj1) {
			bool rem{ obj.get() == obj1.get() };
			if (rem) execOnRemove(obj);
			return rem;
			}), objects.end());
	}

	void update(vec3 _center, vec3 camPos) {
		center = _center;
		Cuboid newBounds{ createBoundsArround(center) };

		deleteOutOfBounds(newBounds);

		for (function<void(Cuboid, Cuboid)> listener : onUpdateListeners) listener(bounds, newBounds);

		for (int i = 0; i < objects.size(); i++) {
			shared_ptr<WorldObject> obj0{ objects[i] };
			vec3 pos0{ obj0->getLocation() };
			float m0{ obj0->getMass() };

			for (int j = i + 1; j < objects.size(); j++) {
				shared_ptr<WorldObject> obj1{ objects[j] };
				vec3 pos1{ obj1->getLocation() };
				float m1{ obj1->getMass() };

				vec3 trans0To1{ pos1 - pos0 };
				float d{ glm::length(trans0To1) };
				trans0To1 = glm::normalize(trans0To1);

				if (d < (obj0->getSize() + obj1->getSize()) / 2) {
					vec3 speed0{ obj0->getSpeed() };
					vec3 speed1{ obj1->getSpeed() };

					float v0{ glm::dot(trans0To1 , speed0) };
					float v1{ glm::dot(trans0To1 , speed1) };

					if (v0 > 0 && v1 < 0 || v0>0 && v0 > v1 || v0 < 0 && v1 < v0) {
						int type{ COLL_TYPE_DEFAULT };

						bool obj0Has{ obj0->hasCollisionListener() };
						bool obj1Has{ obj1->hasCollisionListener() };

						if (obj0Has && obj1Has) {
							type = obj0->execOnCollision(obj1->type, obj1);
							int type1 = obj1->execOnCollision(obj1->type, obj1);
							if (type != type1 || type == COLL_TYPE_DELETE_OTHER) {
								std::cout << "incompatible or invalid collision types: " << type << ", " << type1 << "\n";
								type = 0;
							}
						}
						else if (obj0Has) type = obj0->execOnCollision(obj1->type, obj1);
						else if (obj1Has) type = obj1->execOnCollision(obj0->type, obj1);

						if (type == COLL_TYPE_BOUNCE_OFF) {
							speed0 -= v0 * trans0To1;
							speed1 -= v1 * trans0To1;

							float v0n{ (m0 * v0 + m1 * (2 * v1 - v0)) / (m0 + m1) };
							float v1n{ (m1 * v1 + m0 * (2 * v0 - v1)) / (m0 + m1) };
							speed0 += v0n * trans0To1;
							speed1 += v1n * trans0To1;
							obj0->setSpeed(speed0);
							obj1->setSpeed(speed1);
						}
						else if (type == COLL_TYPE_DELETE_OTHER) {
							if (obj0Has) {
								remove(obj1);
								j--;
							}
							else {
								remove(obj0);
								i--;
								break;
							}
						}
					}

				}

			}

		}

		bounds = newBounds;
	}

	Cuboid getBounds() {
		return bounds;
	}

	void addObject(shared_ptr<WorldObject> worldObject) {
		objects.push_back(worldObject);
		worldObject->init(shared_from_this());
		animationManager->add(worldObject);
		modelManager->add(worldObject->getModel());
	}

	template<class T, class... Types>
	void add(Types ... args) {
		addObject(make_shared<T>(args...));
	}

	shared_ptr<ModelManager> getMM() { return modelManager; }

	shared_ptr<AnimationManager> getAM() { return animationManager; }

	template<typename... Types>
	void updateMM(Types...args) { modelManager->update(args...); }
};

#endif