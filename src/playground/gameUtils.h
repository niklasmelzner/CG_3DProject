#ifndef GAME_UTILS_H
#define GAME_UTILS_H
#include "model.h";
#include <glm/gtx/rotate_vector.hpp>
#include <chrono>

#include "tools.h"
#include "world.h"
#include "gameData.h"

using namespace glm;
using namespace std::chrono;

static shared_ptr<ModelBase> spaceShipModel{ NULL };

class SpaceShip : public WorldObject {
	vec4 viewDirection{ vec4(1, 0, 0,1) };
	vec4 up{ vec4(0,1,0,1) };
	vec4 cam{ vec4(-3,1,0,1) };
	shared_ptr<Transformation> transformation;
	const float DURATION_TILT{ 500 }, SPEED_Y_MAX{ 15 }, SPEED_Z_MAX{ SPEED_Y_MAX }, SPEED_X{ 20 };
	tools::AnimatedTiltValue tiltValueY{ DURATION_TILT }, tiltValueZ{ DURATION_TILT };
public:

	vec4 getViewDirection() {
		return viewDirection;
	}

	vec3 getCamPos() {
		return getLocation() + vec3(cam);
	}

	vec3 getPos() {
		return getLocation();
	}

	SpaceShip() : WorldObject(WO_TYPE_SPACE_SHIP, 10) {
		if (spaceShipModel == NULL) {
			spaceShipModel = loadModel(loadOBJ("./models/Starship2/Falcon t45 Rescue ship flyingEdit.obj"));
			vec3 center{ spaceShipModel->getCenter() };
			shared_ptr<Transformation> baseTrans{ spaceShipModel->newBaseTransformation() };
			baseTrans->translate(-center);
			baseTrans->scale(0.007);
			baseTrans->rotate(PI / 2, vec3(0, 1, 0));
			baseTrans->rotate(-0.2, vec3(0, 0, 1));
		}
		setModelInstance(make_shared<ModelInstance>(spaceShipModel));
		setSize(getModel());
		getModel()->newModelTransformation()->translate(vec3(0, -0.8, 0));
		transformation = getModel()->newModelTransformation(0);

		setDeletable(false);
		onCollision([=](int type, shared_ptr<WorldObject> obj) {

			return World::COLL_TYPE_DELETE_OTHER;
			});
	}

	void update(float f)override {
		transformation->reset();

		float prgZ{ tiltValueZ.get() }, prgY{ tiltValueY.get() };

		vec3 speed{ SPEED_X,0,0 };
		if (prgZ != 0) {
			prgZ = sin(PI / 2 * prgZ);
			transformation->rotate(prgZ * PI / 8, vec3(viewDirection));
			transformation->rotate(-prgZ * PI / 20, vec3(up));
			speed.z = prgZ * SPEED_Z_MAX;
		}

		if (prgY != 0) {
			prgY = sin(PI / 2 * prgY);

			transformation->rotate(prgY * PI / 8, cross(vec3(viewDirection), vec3(up)) * mat3(glm::rotate((float)(-prgZ * PI / 8), vec3(1, 0, 0))));
			speed.y = prgY * SPEED_Y_MAX;
		}

		setSpeed(speed);

		tiltValueY.setMoveMode(0);
		tiltValueZ.setMoveMode(0);
	}

	void moveZ(int mode) {
		tiltValueZ.add(mode);
	}

	void moveY(int mode) {
		tiltValueY.add(mode);
	}

	mat4 getView() {
		vec3 location{ getLocation() };
		return lookAt(
			location + vec3(cam),
			location,
			vec3(up)
		);
	}

};

static float fRandBetween(float start, float end) {
	if (start > end)return fRandBetween(end, start);
	return start + rand() % 1000 / 1000.0f * (end - start);
};

static vec3 randNormVec3() {
	return normalize(vec3{ fRandBetween(-1,1),fRandBetween(-1,1) ,fRandBetween(-1,1) });
}

class Asteroid :public WorldObject {
	shared_ptr<Transformation> resetTransformation, staticTransformation;
	high_resolution_clock::time_point tBirth{ high_resolution_clock::now() };
	float size;

public:
	Asteroid(shared_ptr<ModelInstance> model, vec3 pos, float _size) :
		WorldObject(WO_TYPE_ASTEROID, model, pos, _size), size(_size) {
		staticTransformation = model->newModelTransformation();
		resetTransformation = model->newModelTransformation();
		resetTransformation->scale(0);

		float s{ rand() % 20 == 0 ? 10.0f : fRandBetween(0.3f, 3.0f) };
		setSpeed(s * randNormVec3());
		setRotationSpeed(fRandBetween(0.0f, 0.25f), randNormVec3());
	}

	void update(float f) override {
		resetTransformation->reset();
		float prg{ std::min(1.0f,duration_cast<milliseconds>((high_resolution_clock::now() - tBirth)).count() / ASTEROID_DURATION_POPUP) };
		prg = sin(prg * PI / 2);

		setSize(prg * size);

		resetTransformation->scale(prg * size);
	}

};

class WorldManager {
	shared_ptr<World> world;
	vector<shared_ptr<ModelBase>> asteroidModels{};
	int asteroidCount{ 0 };
	bool initialized{ false };

public:
	WorldManager(shared_ptr<World> _world) :world(_world) {
		world->onDelete(WO_TYPE_ASTEROID, [=](const shared_ptr<const WorldObject> obj) {
			asteroidCount--;
			});
		world->onUpdate([=](const Cuboid& oldBounds, const  Cuboid& newBounds) {
			if (!initialized) {
				initAsteroids(newBounds);
				initialized = true;
			}
			else {
				addAsteroidsInSpace(oldBounds, newBounds);
			}
			});
	}

	void addAsteroidsInSpace(const Cuboid& oldBounds, const Cuboid& newBounds) {
		while (asteroidCount < ASTEROID_COUNT)addAsteroidInSpace(oldBounds, newBounds);
	}

	void addAsteroidInSpace(const Cuboid& oldBounds, const Cuboid& newBounds) {
		int directionX{ 0 }, directionY{ 1 }, directionZ{ 2 };
		//vec4(direction, min, max, weight)
		vector<vec4> ranges{};

		if (newBounds.p0.x < oldBounds.p0.x)
			ranges.push_back(vec4(directionX, newBounds.p0.x, oldBounds.p0.x, oldBounds.size.y * oldBounds.size.z * (oldBounds.p0.x - newBounds.p0.x)));
		else if (newBounds.p1.x > oldBounds.p1.x)
			ranges.push_back(vec4(directionX, oldBounds.p1.x, newBounds.p1.x, oldBounds.size.y * oldBounds.size.z * (newBounds.p1.x - oldBounds.p1.x)));
		if (newBounds.p0.y < oldBounds.p0.y)
			ranges.push_back(vec4(directionY, newBounds.p0.y, oldBounds.p0.y, oldBounds.size.x * oldBounds.size.z * (oldBounds.p0.y - newBounds.p0.y)));
		else if (newBounds.p1.y > oldBounds.p1.y)
			ranges.push_back(vec4(directionY, oldBounds.p1.y, newBounds.p1.y, oldBounds.size.x * oldBounds.size.z * (newBounds.p1.y - oldBounds.p1.y)));
		if (newBounds.p0.z < oldBounds.p0.z)
			ranges.push_back(vec4(directionZ, newBounds.p0.z, oldBounds.p0.z, oldBounds.size.x * oldBounds.size.y * (oldBounds.p0.z - newBounds.p0.z)));
		else if (newBounds.p1.z > oldBounds.p1.z)
			ranges.push_back(vec4(directionZ, oldBounds.p1.z, newBounds.p1.z, oldBounds.size.x * oldBounds.size.y * (newBounds.p1.z - oldBounds.p1.z)));

		if (ranges.size() > 0) {
			float totalWeight{ 0 };

			for (vec4 range : ranges) totalWeight += range[3];

			float p{ fRandBetween(0,totalWeight) };

			float currWeight{ 0 };

			vec4 endRange;
			bool done{ false };
			for (vec4 range : ranges) {
				if (currWeight + range[3] > p) {
					endRange = range;
					done = true;
					break;
				}

				currWeight += range[3];
			}

			if (!done) endRange = ranges[ranges.size() - 1];

			vec3 pos;
			if (endRange[0] == directionX) {
				pos = { fRandBetween(endRange[1],endRange[2]),fRandBetween(newBounds.p0.y,newBounds.p1.y),fRandBetween(newBounds.p0.z,newBounds.p1.z) };
			}
			else if (endRange[0] == directionY) {
				pos = { fRandBetween(newBounds.p0.x,newBounds.p1.x),fRandBetween(endRange[1],endRange[2]),fRandBetween(newBounds.p0.z,newBounds.p1.z) };
			}
			else if (endRange[0] == directionZ) {
				pos = { fRandBetween(newBounds.p0.x,newBounds.p1.x),fRandBetween(newBounds.p0.y,newBounds.p1.y),fRandBetween(endRange[1],endRange[2]) };
			}
			else throw 20;

			createAsteroid(pos);
		}
		else {
			float fX{ (rand() % 1000) / 999.0f };
			float fY{ (rand() % 1000) / 999.0f };
			float fZ{ (rand() % 1000) / 999.0f };

			createAsteroid(vec3(
				newBounds.p0.x + fX * newBounds.size.x,
				newBounds.p0.y + fY * newBounds.size.y,
				newBounds.p0.z + fZ * newBounds.size.z
			));
		}
	}

	void addAsteroidModel(shared_ptr<ModelBase> asteroidModel) {
		vec3 size{ asteroidModel->getSize() };
		asteroidModel->newBaseTransformation()->scale(3.0f / (size.x + size.y + size.z));
		asteroidModels.push_back(asteroidModel);
	}

	void initAsteroids(Cuboid bounds) {
		while (asteroidCount < ASTEROID_COUNT) {
			float fX{ (rand() % 1000) / 999.0f };
			float fY{ (rand() % 1000) / 999.0f };
			float fZ{ (rand() % 1000) / 999.0f };

			createAsteroid(vec3(
				bounds.p0.x + fX * bounds.size.x,
				bounds.p0.y + fY * bounds.size.y,
				bounds.p0.z + fZ * bounds.size.z
			));
		}
	}

	void createAsteroid(vec3 pos) {
		shared_ptr<ModelBase> model{ asteroidModels[rand() % asteroidModels.size()] };

		float size{ ASTEROID_SIZE_MIN + (rand() % 1000) / 999.0f * (ASTEROID_SIZE_MAX - ASTEROID_SIZE_MIN) };

		shared_ptr<ModelInstance> modelInstance{ make_shared<ModelInstance>(model) };

		world->add<Asteroid>(modelInstance, pos, size);

		asteroidCount++;
	}

	void update(vec3 center, vec3 camPos) {
		world->update(center, camPos);
	}

};

#endif