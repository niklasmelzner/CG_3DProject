#include "world.h"

namespace eng {
	double PI = 3.141592653589793238462643383279;

	void WorldObject::setDensity(float _density) {
		mass *= _density / density;
		density = _density;
	}

	void WorldObject::setSize(float _size) {
		size = _size;
		if (calcMass) mass = PI * size * size * size / 6 * density;
	}

	void WorldObject::setModelInstance(shared_ptr<ModelInstance> _model) {
		if (model != nullptr) {
			std::cout << "model instance has already been set\n";
			throw 1;
		}
		model = _model;
	}

	void WorldObject::submitStep(long dt) {
		float f{ dt / 1000.0f };
		update(f);

		location += speed * f;
		model->getModelTransformation()->rotate(f * rotationSpeed, rotationAxis);
		model->getModelTransformation()->setTranslation(location);
	}

	void World::execOnRemove(shared_ptr<WorldObject> obj) {
		if (onDeleteListeners.count(obj->type)) {
			for (function<void(shared_ptr<WorldObject>)> listener : onDeleteListeners[obj->type]) {
				listener(obj);
			}
		}
		obj->dieDelete();
	}

	World::World(vec3 _size, vec3 _offset) :
		size(_size), offset(_offset - _size / 2.0f),
		bounds(createBoundsArround(center)) {
	}

	void World::deleteOutOfBoundsOrNearPos(Cuboid bounds, vec3 pos) {
		objects.erase(remove_if(objects.begin(), objects.end(), [&](shared_ptr<WorldObject> obj) {
			bool rem{ obj->isDeletable() && !bounds.contains(obj->getLocation()) || obj->doDie };
			if (!rem)rem = length(pos - obj->getLocation()) < obj->getSize() / 2;
			if (rem) execOnRemove(obj);
			return rem;
			}), objects.end());
	}

	void World::remove(shared_ptr<WorldObject> obj) {
		objects.erase(remove_if(objects.begin(), objects.end(), [&](shared_ptr<WorldObject> obj1) {
			bool rem{ obj.get() == obj1.get() };
			if (rem) execOnRemove(obj);
			return rem;
			}), objects.end());
	}

	bool World::willCollide(vec3 pos, float size) {
		for (int i = 0; i < objects.size(); i++) {
			shared_ptr<WorldObject> obj{ objects[i] };

			float d{ glm::length(obj->getLocation() - pos) };

			if (d * 2 < obj->getSize() + size) return true;
		}
		return false;
	}

	void World::update(float dt, vec3 _center, vec3 camPos) {
		updateAM(dt);
		center = _center;
		Cuboid newBounds{ createBoundsArround(center) };

		deleteOutOfBoundsOrNearPos(newBounds, camPos);

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
							int type1 = obj1->execOnCollision(obj0->type, obj0);
							if (type != type1 || type == COLL_TYPE_DELETE_OTHER) {
								std::cout << "incompatible or invalid collision types: " << type << ", " << type1 << "\n";
								type = 0;
							}
						}
						else if (obj0Has) type = obj0->execOnCollision(obj1->type, obj1);
						else if (obj1Has) type = obj1->execOnCollision(obj0->type, obj0);

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
						else if (type == COLL_TYPE_DELETE_BOTH) {
							remove(obj1);
							remove(obj0);
							j--;
							i--;
							break;
						}
						else if (type == COLL_TYPE_DELETE_THIS) {
							if (obj0Has) {
								remove(obj0);
								i--;
							}
							else {
								remove(obj1);
								j--;
								break;
							}
						}
					}

				}

			}

		}

		bounds = newBounds;
	}

}