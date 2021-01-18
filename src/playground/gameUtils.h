#ifndef GAME_UTILS_H
#define GAME_UTILS_H
#include <glm/gtx/rotate_vector.hpp>
#include <chrono>

#include "engine/modelLoader.h"
#include "engine/tools.h"
#include "gameData.h"
#include <thread>

using namespace glm;
using namespace std::chrono;
using namespace eng;

static shared_ptr<ModelBase> projectileModel, spaceShipModel;

void initModels();

float fRandBetween(float start, float end);

inline vec3 randNormVec3() {
	return normalize(vec3{ fRandBetween(-1,1),fRandBetween(-1,1) ,fRandBetween(-1,1) });
}

class Asteroid :public WorldObject {
	shared_ptr<Transformation> resetTransformation;
	float size;
	float tSinceBirth{ 0 };
	bool exploding{ false };
	shared_ptr<LightSource> lightExplode;
	vec3 colorExplode{ vec3(1.0, 0.7, 0.2) };
	float massBeforeExplosion{ 0 };
public:

	Asteroid(shared_ptr<ModelInstance> model, vec3 pos, float _size);

	void explode();

	void update(float f) override;

};

struct Projectile : public WorldObject {
	Projectile(vec3 pos, vec3 direction);
};

class SpaceShip : public WorldObject {
	vec4 viewDirection{ vec4(1, 0, 0,1) };
	vec4 up{ vec4(0,1,0,1) };
	vec4 cam{ vec4(-3,1,0,1) };
	shared_ptr<Transformation> transformation;
	const float DURATION_TILT{ 500 }, SPEED_Y_MAX{ 15 }, SPEED_Z_MAX{ SPEED_Y_MAX }, SPEED_X{ 20 };
	tools::AnimatedTiltValue tiltValueY{ DURATION_TILT }, tiltValueZ{ DURATION_TILT };
	shared_ptr<Effect> effectDamage{ make_shared<Effect>(0,vec3(1,1,1),vec3(),Effect::STATIC) };
	int damageCount{ 0 };
	shared_ptr<LightSource> light{ make_shared<LightSource>(LightSource::TYPE_DOT) }, spotLight{ make_shared<LightSource>(LightSource::TYPE_SPOT) };
	shared_ptr<LightSource> camLight{ make_shared<LightSource>(LightSource::TYPE_DOT) };
	bool spaceShipDie{ false };
	float dieCamLightStrength, dieLightStrength;
	high_resolution_clock::time_point tDieStart;

public:

	SpaceShip();

	inline vec4 getViewDirection() { return viewDirection; }

	inline vec3 getCamPos() { return getLocation() + vec3(cam); }

	inline vec3 getPos() { return getLocation(); }

	inline void moveZ(int mode) { tiltValueZ.add(mode); }

	inline void moveY(int mode) { tiltValueY.add(mode); }

	void update(float f) override;

	mat4 getView();

};

class WorldManager {
	shared_ptr<World> world;
	vector<shared_ptr<ModelBase>> asteroidModels{};
	vector<int> spawnWeights{};
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
		while (asteroidCount < ASTEROID_COUNT) addAsteroidInSpace(oldBounds, newBounds);
	}

	void addAsteroidInSpace(const Cuboid& oldBounds, const Cuboid& newBounds) {
		int directionX{ 0 }, directionY{ 1 }, directionZ{ 2 };
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
			do {
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
			} while (!createAsteroid(pos));
		}
		else {
			float fX, fY, fZ;
			do {
				fX = (rand() % 1000) / 999.0f;
				fY = (rand() % 1000) / 999.0f;
				fZ = (rand() % 1000) / 999.0f;
			} while (!createAsteroid(vec3(
				newBounds.p0.x + fX * newBounds.size.x,
				newBounds.p0.y + fY * newBounds.size.y,
				newBounds.p0.z + fZ * newBounds.size.z
			)));
		}
	}

	void addAsteroidModel(shared_ptr<ModelBase> asteroidModel, float spawnWeight) {
		vec3 size{ asteroidModel->getSize() };
		shared_ptr<Transformation> tr{ asteroidModel->newBaseTransformation() };
		tr->translate(-asteroidModel->getCenter());
		tr->scale(1.0f / asteroidModel->getAVGSize());
		asteroidModels.push_back(asteroidModel);
		spawnWeights.push_back(spawnWeight);
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

	high_resolution_clock::time_point tLastUpdate = high_resolution_clock::now();

	int getRandomAsteroidModelIndex() {
		float weightSum{ 0 };
		for (float weight : spawnWeights) weightSum += weight;
		float f = rand() % 1000 / 999.0f * weightSum;
		for (int i = 0; i < spawnWeights.size(); i++) {
			if (spawnWeights[i] > f)return i;
			else f -= spawnWeights[i];
		}

		return spawnWeights.size() - 1;
	}

	bool createAsteroid(vec3 pos) {
		if (length(pos) < 50) return false;

		float size{ ASTEROID_SIZE_MIN + (rand() % 1000) / 999.0f * (ASTEROID_SIZE_MAX - ASTEROID_SIZE_MIN) };

		if (world->willCollide(pos, size))return false;

		shared_ptr<ModelBase> model{ asteroidModels[getRandomAsteroidModelIndex()] };

		shared_ptr<ModelInstance> modelInstance{ make_shared<ModelInstance>(model) };

		world->add<Asteroid>(modelInstance, pos, size);

		asteroidCount++;

		return true;
	}

	void update(long dt, vec3 center, vec3 camPos) {

		world->update(dt, center, camPos);

	}

};

#endif