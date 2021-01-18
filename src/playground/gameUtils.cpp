#include "gameUtils.h"

void initModels() {
	spaceShipModel = []() {
		shared_ptr<ModelBase> model{ loadModel(loadOBJ("./models/Starship/Falcon t45 Rescue ship flyingEdit.obj")) };
		vec3 center{ model->getCenter() };
		shared_ptr<Transformation> baseTrans{ model->newBaseTransformation() };
		baseTrans->translate(-center);
		baseTrans->scale(0.007);
		baseTrans->rotate(PI / 2, vec3(0, 1, 0));
		baseTrans->rotate(-0.17, vec3(0, 0, 1));
		return model;
	}();
	projectileModel = []() {
		shared_ptr<ModelBase> model = loadModel(loadOBJ("./models/Projectile/projectile.obj", true));
		vec3 center{ model->getCenter() };
		shared_ptr<Transformation> baseTrans{ model->newBaseTransformation() };
		baseTrans->translate(-center);
		baseTrans->scale(0.07, 0.2, 0.07);
		baseTrans->rotate(PI / 2, vec3(0, 0, 1));
		return model;
	}();
}

float fRandBetween(float start, float end) {
	if (start > end) return fRandBetween(end, start);
	return start + rand() % 1000 / 1000.0f * (end - start);
};

Asteroid::Asteroid(shared_ptr<ModelInstance> model, vec3 pos, float _size) :
	WorldObject(WO_TYPE_ASTEROID, model, pos, _size), size(_size) {
	resetTransformation = model->newModelTransformation();
	resetTransformation->scale(0);

	float s{ rand() % 20 == 0 ? 10.0f : fRandBetween(0.3f, 3.0f) };
	setSpeed(s * randNormVec3());
	setRotationSpeed(fRandBetween(0.02f, 0.3f), randNormVec3());
}

void Asteroid::explode() {
	exploding = true;
	tSinceBirth = 0;
	addEffect(make_shared<Effect>(0, vec3(1.0, 1.0, 1.0), vec3(1.0, 0.7, 0.2), Effect::LINEAR, ASTEROID_DURATION_EXPLODE * 1.5));
	lightExplode = make_shared<LightSource>(LightSource::TYPE_DOT);

	lightExplode->setDistance(10.0f);
	lightExplode->setColor(colorExplode);
	lightExplode->setStrength(6 * size);
	lightExplode->transformationSupplier([=]() {
		return getModel()->getTransformEnd();
		});
	getModel()->addLight(lightExplode);

	massBeforeExplosion = getMass();
}

void Asteroid::update(float f) {
	tSinceBirth += f;
	resetTransformation->reset();
	if (exploding) {
		float prg{ std::min(1.0f,tSinceBirth * 1000 / ASTEROID_DURATION_EXPLODE) };
		prg = sin((1 - prg) * PI / 2);
		float prgScale;
		float d = 0.9;

		if (prg > d) {
			prgScale = 2 - prg;
		}
		else {
			prgScale = prg / d * (2 - d);
		}

		setSize(prgScale * size);
		resetTransformation->scale(prgScale * size);
		lightExplode->setColor(colorExplode * prgScale * (2 - d));

		if (prg == 0.0) {

			float strength{ 1000.0f * massBeforeExplosion };
			vec3 pos{ getLocation() };

			getWorld()->forEachObject([=](shared_ptr<WorldObject> obj) {
				if (obj->type == WO_TYPE_ASTEROID) {
					vec3 direction{ obj->getLocation() - pos };
					float distance{ length(direction) };
					direction = normalize(direction);
					obj->setSpeed(obj->getSpeed() + direction * strength / (obj->getMass() * distance * distance));
				}
				});

			die();
		}
	}
	else {
		float prg{ std::min(1.0f,tSinceBirth * 1000 / ASTEROID_DURATION_POPUP) };
		prg = sin(prg * PI / 2);
		setSize(prg * size);

		resetTransformation->scale(prg * size);
	}
}

Projectile::Projectile(vec3 pos, vec3 direction) :WorldObject(WO_TYPE_PROJECTILE, make_shared<ModelInstance>(projectileModel)) {
	setSize(getModel());

	onCollision([=](int type, shared_ptr<WorldObject> obj) {
		if (type != WO_TYPE_ASTEROID) return 0;
		Asteroid* asteroid = reinterpret_cast<Asteroid*>(obj.get());
		asteroid->explode();

		return World::COLL_TYPE_DELETE_THIS;
		});
	setLocation(pos);
	setSpeed(100.0f * direction);

	shared_ptr<LightSource> lightSource{ make_shared<LightSource>(LightSource::TYPE_DOT) };

	lightSource->setColor(1.0, 0.7, 0.2);
	lightSource->setStrength(6);
	lightSource->setDistance(2.0f);
	lightSource->translationSupplier([=]() {
		return getLocation();
		});
	getModel()->addLight(lightSource);
}

SpaceShip::SpaceShip() : WorldObject(WO_TYPE_SPACE_SHIP, make_shared<ModelInstance>(spaceShipModel), 10) {
	setSize(getModel());
	getModel()->newModelTransformation()->translate(vec3(0, -0.8, 0));
	transformation = getModel()->newModelTransformation(0);

	light->setDistance(100.0f);
	light->setColor(1.0f, 1.0f, 1.0f);
	light->setStrength(dieLightStrength = 0.7f);
	light->translationSupplier([=]() {return getLocation(); });
	light->setPos(vec3(1, 1, 0));
	getModel()->addLight(light);

	camLight->setDistance(100.0f);
	camLight->setStrength(dieCamLightStrength = 0.3f);
	camLight->setColor(1.0f, 1.0f, 1.0f);
	camLight->translationSupplier([=]() {return getLocation(); });
	camLight->setPos(vec3(cam));
	getModel()->addLight(camLight);

	spotLight->setDistance(3000.0f);
	spotLight->setColor(1.0f, 0.95f, 0.9f);
	spotLight->setAngle(radians(32.0f));
	spotLight->setDirection(0.0f, 0.0f, 1.0f);
	spotLight->setStrength(1.3);
	spotLight->transformationSupplier([=]() {return getModel()->getTransformEnd(); });
	getModel()->addLight(spotLight);

	addEffect(effectDamage);

	setDeletable(false);
	onCollision([=](int type, shared_ptr<WorldObject> obj) {
		if (type == WO_TYPE_ASTEROID) {
			damageCount++;
			effectDamage->value0 -= vec3(0.1, 0.4, 0.5) * (1.0f * damageCount / MAX_COLLISIONS);
			if (damageCount > MAX_COLLISIONS && !spaceShipDie) {
				spaceShipDie = true;
				tDieStart = high_resolution_clock::now();
			}
			return World::COLL_TYPE_DELETE_OTHER;
		}
		return 0;
		});
}

void SpaceShip::update(float f) {
	transformation->reset();

	vec3 speed{ SPEED_X,0,0 };

	if (spaceShipDie) {
		float prg{ std::min(1.0f,duration_cast<milliseconds>(high_resolution_clock::now() - tDieStart).count() / SPACESHIP_DURATION_EXPLODE) };
		prg = sin(PI / 2 * prg);

		transformation->rotate(prg * PI / 2, vec3(0, 0, -1));

		transformation->translate(vec3(0, -5, 0) * prg);

		camLight->setStrength(dieCamLightStrength * (1 - prg));
		light->setStrength(dieLightStrength * (1 - prg));

		speed.x *= (1 - prg);

		if (prg == 1.0) die();
	}

	float prgZ{ tiltValueZ.get() }, prgY{ tiltValueY.get() };

	if (prgZ != 0) {
		prgZ = sin(PI / 2 * prgZ);
		transformation->rotate(prgZ * PI / 8, vec3(viewDirection));
		transformation->rotate(-prgZ * PI / 15, vec3(up));
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

mat4 SpaceShip::getView() {
	vec3 location{ getLocation() };
	return lookAt(
		location + vec3(cam),
		location,
		vec3(up)
	);
}