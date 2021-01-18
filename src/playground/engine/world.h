#ifndef WORLD_H
#define WORLD_H

#include "model.h"
#include <map>
#include <functional>

namespace eng {
	extern double PI;

	class Cuboid {
	public:
		vec3 p0, p1, size;
		Cuboid() {}
		Cuboid(vec3 _pos, vec3 _size) :p0(_pos), p1(_pos + _size), size(_size) {}

		inline bool contains(vec3 _pos) {
			return p0.x <= _pos.x && _pos.x < p1.x&& p0.y <= _pos.y && _pos.y < p1.y&& p0.z <= _pos.z && _pos.z < p1.z;
		}

		inline bool operator !=(Cuboid other) {
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
		bool doDie{ false };

		WorldObject(int type, shared_ptr<ModelInstance> model, vec3 pos, float size) :
			WorldObject(type, model, pos, size, PI* size* size* size / 6) {
			calcMass = true;
		}

		WorldObject(int _type, shared_ptr<ModelInstance> _model, vec3 _pos, float _size, float _mass) :AnimatedObject(),
			model(_model), location(_pos), size(_size), mass(_mass), type(_type) {
			calcMass = false;
		}

		WorldObject(int type, shared_ptr<ModelInstance> model, float size = 1) :WorldObject(type, model, vec3{}, size) {};

		WorldObject(int type, float _size = 1) :WorldObject(type, nullptr, vec3{}, size) {}

		inline void addEffect(shared_ptr<Effect> effect) { model->addEffect(effect); }
		
		inline void removeEffect(shared_ptr<Effect> effect) { model->removeEffect(effect); }

		inline void die() { doDie = true; }

		inline shared_ptr<World> getWorld() { return world; }

		inline bool hasCollisionListener() { return collisionListenerSet; }

		void setDensity(float _density);

		inline int execOnCollision(int type, shared_ptr<WorldObject> obj) { return onCollisionListener(type, obj); }

		inline void onCollision(function<int(int type, shared_ptr<WorldObject>)> listener) {
			onCollisionListener = listener;
			collisionListenerSet = true;
		}

		void setSize(float _size);

		inline float getSize() { return size; }

		inline void setSize(shared_ptr<ModelInstance> model) { setSize(model->model->getAVGSize()); }

		inline void setDeletable(bool _deletable) { deletable = _deletable; }

		inline bool isDeletable() { return deletable; };

		inline void dieDelete() {
			removeFromAnimationManager();
			model->removeFromModelManager();
		}

		inline void init(shared_ptr<World> _world) { world = _world; }

		void setModelInstance(shared_ptr<ModelInstance> _model);

		inline shared_ptr<ModelInstance> getModel() { return model; }

		inline void setSpeed(vec3 _speed) { speed = _speed; }

		inline vec3 getSpeed() { return speed; }

		inline float getMass() { return mass; }

		inline void setRotationSpeed(float speed, vec3 axis) {
			rotationSpeed = speed;
			rotationAxis = axis;
		}

		inline vec3 getLocation() { return location; }

		inline void setLocation(vec3 _location) { location = _location; }

		void submitStep(long dt) override;

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

		void execOnRemove(shared_ptr<WorldObject> obj);
	public:
		static const int COLL_TYPE_BOUNCE_OFF{ 1 }, COLL_TYPE_DELETE_OTHER{ 2 },
			COLL_TYPE_DELETE_BOTH{ 3 }, COLL_TYPE_DEFAULT{ COLL_TYPE_BOUNCE_OFF },
			COLL_TYPE_DELETE_THIS{ 4 };

		World(vec3 _size, vec3 _offset);

		inline void forEachObject(function<void(shared_ptr<WorldObject>)> f) {
			for (shared_ptr<WorldObject> obj : objects) f(obj);
		}

		inline void onDelete(int type, std::function<void(const shared_ptr<const WorldObject>)> listener) {
			onDeleteListeners[type].push_back(listener);
		}

		inline void onUpdate(std::function<void(const Cuboid&, const Cuboid&)> listener) {
			onUpdateListeners.push_back(listener);
		}

		template<typename... Types>
		inline void updateAM(Types... args) { animationManager->update(args...); }

		inline Cuboid createBoundsArround(vec3 pos) { return Cuboid{ pos + offset,size }; }

		inline Cuboid getBounds() { return bounds; }

		template<class T, class... Types>
		inline void add(Types ... args) { addObject(make_shared<T>(args...)); }

		inline shared_ptr<ModelManager> getMM() { return modelManager; }

		inline shared_ptr<AnimationManager> getAM() { return animationManager; }

		template<typename... Types>
		void updateMM(Types...args) { modelManager->update(args...); }

		void deleteOutOfBoundsOrNearPos(Cuboid bounds, vec3 pos);

		void remove(shared_ptr<WorldObject> obj);

		bool willCollide(vec3 pos, float size);

		void update(float dt, vec3 _center, vec3 camPos);

		void addObject(shared_ptr<WorldObject> worldObject) {
			objects.push_back(worldObject);
			worldObject->init(shared_from_this());
			animationManager->add(worldObject);
			modelManager->add(worldObject->getModel());
		}

	};

}

#endif