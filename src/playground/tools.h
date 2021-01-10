#ifndef TOOLS_H
#define TOOLS_H

#include <chrono>

using namespace std::chrono;

namespace tools {

	class AnimatedTiltValue {
		static const int MOVE_STATE_IDLE{ 0 }, MOVE_STATE_NEG{ 1 }, MOVE_STATE_POS{ 2 }, MOVE_STATE_NEG_BACK{ 3 }, MOVE_STATE_POS_BACK{ 4 };
		float durationTilt;
		int moveMode{ 0 }, moveState{ MOVE_STATE_IDLE };
		float prgOffset;
		high_resolution_clock::time_point tP;

		void changeMoveState(int nextState, float nextPrgOffset) {
			moveState = nextState;
			prgOffset = std::min(1.0f, std::max(-1.0f, nextPrgOffset));
			tP = high_resolution_clock::now();
		}

	public:
		AnimatedTiltValue(float _durationTilt) :durationTilt(_durationTilt) {}

		void add(const int mode) {
			moveMode += mode;
		}

		void setMoveMode(const int mode) {
			moveMode = mode;
		}

		float get() {
			float prg = 0;
			if (moveState == MOVE_STATE_IDLE) {
				if (moveMode > 0) changeMoveState(MOVE_STATE_POS, 0);
				else if (moveMode < 0) changeMoveState(MOVE_STATE_NEG, 0);
			}
			if (moveState == MOVE_STATE_POS) {
				prg = -prgOffset + std::min(1.0f, duration_cast<milliseconds>(high_resolution_clock::now() - tP).count() / durationTilt) * (1 + prgOffset);
				if (moveMode != 1)
					changeMoveState(MOVE_STATE_POS_BACK, prg);
			}
			else if (moveState == MOVE_STATE_NEG) {
				prg = prgOffset - std::min(1.0f, duration_cast<milliseconds>(high_resolution_clock::now() - tP).count() / durationTilt) * (1 + prgOffset);
				if (moveMode != -1)
					changeMoveState(MOVE_STATE_NEG_BACK, -prg);
			}
			if (moveState == MOVE_STATE_POS_BACK) {
				prg = std::max(0.0f, prgOffset - duration_cast<milliseconds>(high_resolution_clock::now() - tP).count() / durationTilt * 1.3f);
				if (prg == 0.0) changeMoveState(moveMode == 0 ? MOVE_STATE_IDLE : MOVE_STATE_NEG, 0);
				else if (moveMode == -1)changeMoveState(MOVE_STATE_NEG, prg);

			}
			else if (moveState == MOVE_STATE_NEG_BACK) {
				prg = -std::max(0.0f, prgOffset - duration_cast<milliseconds>(high_resolution_clock::now() - tP).count() / durationTilt * 1.3f);
				if (prg == 0.0) changeMoveState(moveMode == 0 ? MOVE_STATE_IDLE : MOVE_STATE_POS, 0);
				else if (moveMode == 1)changeMoveState(MOVE_STATE_POS, -prg);
			}
			return prg;
		}


	};

}


#endif