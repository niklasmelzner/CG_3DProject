#include "light.h"

namespace eng {

	void LightSource::updateTransform() {
		if (supplierTransformSet) {
			mat4 mat{ supplierTransform() };
			pos = vec3(mat * vec4(posIn, 1.0));
			vec3 dir{ vec3(mat * vec4(dirIn,1.0)) - vec3(mat * vec4(0,0,0,1)) };
			modifiers1.x = dir.x;
			modifiers1.y = dir.y;
			modifiers1.z = dir.z;
		}
		else if (supplierTranslateSet) {
			pos = supplierTranslate() + posIn;
		}
	}

}

