#pragma once

enum class ChangeState {
	None,
	OnGoing,
	Option1,
	Option2,
	Option3,
	Option4,
	Quit
};

namespace NCL {
	namespace CSC8503 {
		class PushDownStateChangeManager {
		private:
			PushDownStateChangeManager(): state (ChangeState::OnGoing) {};
			~PushDownStateChangeManager(){};
			ChangeState state;
		public:
			static PushDownStateChangeManager& instance() {
				static PushDownStateChangeManager INSTANCE;
				return INSTANCE;
			}
			ChangeState GetChangeState() {
				return state;
			};
			void SetChangeState(ChangeState state = ChangeState::OnGoing) {
				this->state = state;
			};
		};
	}
}
