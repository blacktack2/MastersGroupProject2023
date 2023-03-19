/**
 * @file   OptionManager.h
 * @brief  A manager for game options.
 *
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once

namespace NCL {
	namespace CSC8503 {
		class OptionManager {
		private:
			OptionManager();
			~OptionManager();

			bool isSunMove = false;

			bool isSound = true;
			bool isVolumeUp = false;
			bool isVolumeDown = false;

			bool isDebugMode = false;

			int upTimes;
			int downTimes;

			int counter;
		public:
			static OptionManager& instance() {
				static OptionManager INSTANCE;
				return INSTANCE;
			}
			void Update(float dt);

			void SetSunMove(bool flag) { isSunMove = flag; };
			bool GetSunMove() { return isSunMove; };

			void SetDebugMode(bool flag) { isDebugMode = flag; };
			bool GetDebugMode() { return isDebugMode; };

			void SetSound(bool flag) { isSound = flag; };
			bool GetSound() { return isSound; };

			void SetVolumeUp(bool flag) { isVolumeUp = flag; };
			bool GetVolumeUp() { return isVolumeUp; };

			void SetVolumeDown(bool flag) { isVolumeDown = flag; };
			bool GetVolumeDown() { return isVolumeDown; };

			void SetUpTimes(int num) { upTimes = num; };
			int GetUpTimes() { return upTimes; };

			void SetDownTimes(int num) { downTimes = num; };
			int GetDownTimes() { return downTimes; };

			void SetCounter(int num) { counter = num; };
			int GetCounter() { return counter; };
		};
	}
}
