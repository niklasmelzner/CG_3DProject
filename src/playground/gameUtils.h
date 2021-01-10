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

class Asteroid1 :public AnimatedObject {
	shared_ptr<ModelInstance> model;
	shared_ptr<Transformation> transformation, transformation2;
	vec3 position, rotationAxis;
	float size, angleRot{ fRandBetween(0.0f,0.25f) }, mass;
	const float DURATION_POPUP{ 300.0f };
	high_resolution_clock::time_point tBirth{ high_resolution_clock::now() };
public:
	vec3 speed;
	Asteroid1(shared_ptr<ModelInstance> _model, vec3 pos, float _size) :
		model(_model), position(pos), size(_size), mass(PI* size* size* size * 4 / 24) {
		transformation2 = model->newModelTransformation();
		transformation = model->newModelTransformation();
		transformation->scale(size);
		float s{ rand() % 20 == 0 ? 10.0f : fRandBetween(0.3f, 3.0f) };
		speed = s * randNormVec3();
		rotationAxis = randNormVec3();

		transformation2->rotate(fRandBetween(0, PI), rotationAxis);

		submitStep(0);
	}

	inline float getMass() {
		return mass;
	}

	inline float getSize() {
		return size;
	}

	void precalculateStep(long dt) {
		position += speed * (float)(dt / 1000.0f);
	}

	void submitStep(long dt) {
		transformation->reset();
		float prg{ std::min(1.0f,duration_cast<milliseconds>((high_resolution_clock::now() - tBirth)).count() / DURATION_POPUP) };
		prg = sin(prg * PI / 2);

		transformation->setScale(prg * size);

		transformation->setTranslation(position);

		transformation2->rotate(angleRot * dt / 1000.0, rotationAxis);
	}

	inline vec3 getPos() {
		return position;
	}

	void die() {
		removeFromAnimationManager();
		model->removeFromModelManager();
	}

};

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

class World1 :public enable_shared_from_this<World1> {
	shared_ptr<SpaceShip> spaceShip;
	vector<shared_ptr<ModelBase>> asteroidModels{};
	vector<shared_ptr<Asteroid1>> asteroids{};

	const float ASTEROID_MIN_SIZE{ 5 }, ASTEROID_MAX_SIZE{ 15 };//3,13
	static const int WIDTH{ 110 }, HEIGHT{ WIDTH }, DEPTH{ 110 };//110,110
	static const int ASTEROID_COUNT{ 200 };
	Cuboid bounds;
	bool initialized{ false };
	const shared_ptr<ModelManager> modelManager;
	const shared_ptr < AnimationManager> animationManager;

public:
	World1(shared_ptr<SpaceShip> _spaceShip, shared_ptr<ModelManager> _modelManager, shared_ptr<AnimationManager> _animationManager) :
		spaceShip(_spaceShip), modelManager(_modelManager), animationManager(_animationManager) {}

	void addAsteroidModel(shared_ptr<ModelBase> asteroidModel) {
		vec3 size{ asteroidModel->getSize() };
		asteroidModel->newBaseTransformation()->scale(3.0f / (size.x + size.y + size.z));
		asteroidModels.push_back(asteroidModel);
	}

	void createAsteroid(const vec3 pos) {
		shared_ptr<ModelBase> model{ asteroidModels[rand() % asteroidModels.size()] };

		float scale{ ASTEROID_MIN_SIZE + (rand() % 1000) / 999.0f * (ASTEROID_MAX_SIZE - ASTEROID_MIN_SIZE) };

		shared_ptr<ModelInstance> modelInstance{ make_shared<ModelInstance>(model) };
		shared_ptr<Asteroid1> asteroid{ make_shared<Asteroid1>(modelInstance, pos,scale) };

		modelManager->add(modelInstance);
		animationManager->add(asteroid);
		asteroids.push_back(asteroid);
	}

	vec3 camPosLast;

	Cuboid createBoundsArround(vec3 pos) {
		return { vec3(20,0,0) + pos + vec3(0, -HEIGHT / 2, -WIDTH / 2),vec3(DEPTH, HEIGHT, WIDTH) };
	}

	void deleteOutOfBoundsOrNearPos(Cuboid bounds, vec3 pos) {
		asteroids.erase(remove_if(asteroids.begin(), asteroids.end(), [&](shared_ptr<Asteroid1> asteroid) {
			bool rem{
				!bounds.contains(asteroid->getPos()) ||
				glm::length(asteroid->getPos() - pos) <= asteroid->getSize() / 2.2f
			};
			if (rem) asteroid->die();
			return rem;
			}), asteroids.end());
	}

	void initAsteroids(Cuboid bounds) {
		while (asteroids.size() < ASTEROID_COUNT) {
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

	void addAsteroidInSpace(Cuboid newBounds) {
		int directionX{ 0 }, directionY{ 1 }, directionZ{ 2 };
		//vec4(direction, min, max, weight)
		vector<vec4> ranges{};

		if (newBounds.p0.x < bounds.p0.x)
			ranges.push_back(vec4(directionX, newBounds.p0.x, bounds.p0.x, bounds.size.y * bounds.size.z * (bounds.p0.x - newBounds.p0.x)));
		else if (newBounds.p1.x > bounds.p1.x)
			ranges.push_back(vec4(directionX, bounds.p1.x, newBounds.p1.x, bounds.size.y * bounds.size.z * (newBounds.p1.x - bounds.p1.x)));
		if (newBounds.p0.y < bounds.p0.y)
			ranges.push_back(vec4(directionY, newBounds.p0.y, bounds.p0.y, bounds.size.x * bounds.size.z * (bounds.p0.y - newBounds.p0.y)));
		else if (newBounds.p1.y > bounds.p1.y)
			ranges.push_back(vec4(directionY, bounds.p1.y, newBounds.p1.y, bounds.size.x * bounds.size.z * (newBounds.p1.y - bounds.p1.y)));
		if (newBounds.p0.z < bounds.p0.z)
			ranges.push_back(vec4(directionZ, newBounds.p0.z, bounds.p0.z, bounds.size.x * bounds.size.y * (bounds.p0.z - newBounds.p0.z)));
		else if (newBounds.p1.z > bounds.p1.z)
			ranges.push_back(vec4(directionZ, bounds.p1.z, newBounds.p1.z, bounds.size.x * bounds.size.y * (newBounds.p1.z - bounds.p1.z)));

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
				bounds.p0.x + fX * bounds.size.x,
				bounds.p0.y + fY * bounds.size.y,
				bounds.p0.z + fZ * bounds.size.z
			));
		}

	}

	void addAsteroidsInSpace(Cuboid newBounds) {
		while (asteroids.size() < ASTEROID_COUNT)addAsteroidInSpace(newBounds);
	}

	void update() {
		vec3 shipPos{ spaceShip->getPos() }, camPos{ spaceShip->getCamPos() };
		Cuboid newBounds{ createBoundsArround(camPos - vec3(20,0,0)) };
		if (!initialized) {
			initAsteroids(bounds = newBounds);
			initialized = true;
		}
		else {
			deleteOutOfBoundsOrNearPos(newBounds, camPos);
			addAsteroidsInSpace(newBounds);
			bounds = newBounds;
		}

		//collisions

		for (int i = 0; i < asteroids.size(); i++) {
			shared_ptr<Asteroid1> asteroid0{ asteroids[i] };
			vec3 pos0{ asteroid0->getPos() };
			float m0{ asteroid0->getMass() };

			for (int j = i + 1; j < asteroids.size(); j++) {
				shared_ptr<Asteroid1> asteroid1{ asteroids[j] };
				vec3 pos1{ asteroid1->getPos() };
				float m1{ asteroid1->getMass() };

				vec3 trans0To1{ pos1 - pos0 };
				float d{ glm::length(trans0To1) };
				trans0To1 = glm::normalize(trans0To1);

				if (d < (asteroid0->getSize() + asteroid1->getSize()) / 2) {

					vec3 speed0{ asteroid0->speed };
					vec3 speed1{ asteroid1->speed };

					float v0{ glm::dot(trans0To1 , speed0) };
					float v1{ glm::dot(trans0To1 , speed1) };

					if (v0 > 0 && v1 < 0 || v0>0 && v0 > v1 || v0 < 0 && v1 < v0) {
						speed0 -= v0 * trans0To1;
						speed1 -= v1 * trans0To1;

						float v0n{ (m0 * v0 + m1 * (2 * v1 - v0)) / (m0 + m1) };
						float v1n{ (m1 * v1 + m0 * (2 * v0 - v1)) / (m0 + m1) };
						speed0 += v0n * trans0To1;
						speed1 += v1n * trans0To1;
						asteroid0->speed = speed0;
						asteroid1->speed = speed1;
					}

				}

			}

		}
	}

};

#endif