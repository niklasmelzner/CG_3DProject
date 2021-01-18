#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <functional>

using namespace glm;
using namespace std;
namespace eng {

	class LightSource {
	public:
		static const int TYPE_DOT{ 0 }, TYPE_SPOT{ 1 };

		int type;
		vec3 color{ 1.0f,1.0f,1.0f }, pos{};
		vec4 modifiers{ 100.0f,0.0f,0.0f,1.0f }, modifiers1{};

		vec3 posIn{}, dirIn{};
		bool supplierTransformSet{ false }, supplierTranslateSet{ false };
		function<mat4()> supplierTransform;
		function<vec3()> supplierTranslate;

		LightSource(int _type) :type(_type) {}

		void updateTransform();

		inline void LightSource::setPos(vec3 _pos) {
			pos = posIn = _pos;
		}

		inline void LightSource::translationSupplier(function<vec3()> _supplierTranslate) {
			supplierTranslate = _supplierTranslate;
			supplierTranslateSet = true;
		}

		inline void LightSource::transformationSupplier(function<mat4()> _supplierTransform) {
			supplierTransform = _supplierTransform;
			supplierTransformSet = true;
		}

		inline void LightSource::setDistance(float d) {
			modifiers[0] = d;
		}

		inline void LightSource::setDirection(float x, float y, float z) {
			modifiers1.x = x;
			modifiers1.y = y;
			modifiers1.z = z;
			dirIn = { x,y,z };
		}

		inline void LightSource::setDirection(vec3 d) {
			setDirection(d.x, d.y, d.z);
		}

		inline void LightSource::setColor(vec3 c) {
			color = c;
		}

		inline void LightSource::setStrength(float th) {
			modifiers[3] = th;
		}

		inline void LightSource::setColor(float r, float g, float b) {
			setColor(vec3(r, g, b));
		}

		inline void LightSource::setMinD(float d) {
			modifiers[1] = d;
		}

		inline void LightSource::setAmbientStrength(float d) {
			modifiers[2] = d;
		}

		inline void LightSource::setAngle(float a) {
			modifiers1[3] = a;
		}

	};

}

#endif