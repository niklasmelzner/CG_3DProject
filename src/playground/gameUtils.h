#ifndef GAME_UTILS_H
#define GAME_UTILS_H
#include "model.h";
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

class SpaceShipModel : public ModelBase {
public:
	SpaceShipModel(float r, float g, float b) :ModelBase(3, 3) {
		vec3 p0{ vec3(0.5, 0 ,0) };
		vec3 p1{ vec3(-0.5, 0 ,-0.5) };
		vec3 p2{ vec3(-0.5, 0 ,0.5) };
		addFace(p0, p1, p2, vec3(r, g, b));
	}
	
};

static shared_ptr<ModelBase> spaceShipModel{ NULL };

class SpaceShip : public AnimatedObject {

	float defaultAcceleration = 5;
	float maxSpeed = 5;

	vec3 acceleration{};
	vec3 location{};
	vec3 modelOffset{0,0,0};
	vec3 speed{};
	mat3 rotation;
	vec4 viewDirection{ vec4(1, 0, 0,1) };
	vec4 up{ vec4(0,1,0,1) };
	vec4 cam{ vec4(-3,1,0,1) };
	shared_ptr<ModelInstance> model;
	float animationTransformFactor;
	float angleX = 0, angleY = 0, angleZ = 0;

public:

	vec4 getViewDirection() {
		return viewDirection;
	}

	vec3 getCamPos() {
		return location + vec3(cam);
	}

	vec3 getPos() {
		return location;
	}

	SpaceShip() :AnimatedObject() {
		if (spaceShipModel == NULL) {
			spaceShipModel = loadModel( loadOBJ("./models/Starship2/Falcon t45 Rescue ship flying.obj") );
			//spaceShipModel = make_shared<SpaceShipModel>(0.1f, 0.7f, 0.1f);
		}
		model = make_shared<ModelInstance>(spaceShipModel);
		model->scale(0.005, 0.005, 0.005);
		model->rotate(PI/2,vec3(0,1,0));
	}

	void precalculateStep(long dt) {
		float f = dt / 1000.0;
		animationTransformFactor = f;
	}

	void accelerate(long a) {
		acceleration.x += viewDirection.x * (int)a;
		acceleration.y += viewDirection.y * (int)a;
		acceleration.z += viewDirection.z * (int)a;
	}

	void submitStep() {
		rotateAround(angleX, vec3(viewDirection));
		rotateAround(angleY, vec3(up));
		rotateAround(angleZ, cross(vec3(up), vec3(viewDirection)));

		angleX = angleY = angleZ = 0;

		speed += acceleration * defaultAcceleration * animationTransformFactor;

		float l;
		if ((l = length(speed)) > maxSpeed) speed = speed * maxSpeed / l;


		acceleration = vec3(0, 0, 0);

		location = location + speed * animationTransformFactor;

		model->setTranslation(location);
	}

	shared_ptr<ModelInstance> getModelInstance() {
		return model;
	}

	void rotateX(float angle) {
		angleX += angle;
	}

	void rotateY(float angle) {
		angleY += angle;
	}

	void rotateZ(float angle) {
		angleZ += angle;
	}

	void rotateAround(float angle, vec3 axis) {
		mat4 mat = rotate(angle, axis);
		viewDirection = mat * viewDirection;
		up = mat * up;
		cam = mat * cam;

		model->transform(mat);
	}

	mat4 getView() {

		return lookAt(
			location + vec3(cam),
			location,
			vec3(up)
		);
	}

};


#endif