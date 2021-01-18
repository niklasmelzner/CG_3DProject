#ifndef TOOLS_H
#define TOOLS_H

#include <chrono>

using namespace std::chrono;

namespace eng {
	namespace tools {

		class AnimatedTiltValue {
			static const int MOVE_STATE_IDLE{ 0 }, MOVE_STATE_NEG{ 1 }, MOVE_STATE_POS{ 2 }, MOVE_STATE_NEG_BACK{ 3 }, MOVE_STATE_POS_BACK{ 4 };
			float durationTilt;
			int moveMode{ 0 }, moveState{ MOVE_STATE_IDLE };
			float prgOffset{0};
			high_resolution_clock::time_point tP;

			void changeMoveState(int nextState, float nextPrgOffset);

		public:
			AnimatedTiltValue(float _durationTilt) :durationTilt(_durationTilt) {}

			inline void add(const int mode) {
				moveMode += mode;
			}

			inline void setMoveMode(const int mode) {
				moveMode = mode;
			}

			float get();

		};

	}
}

#endif