
#include "Window.h"
#include <functional>
#include <thread>
#include <chrono>
#include <iostream>

namespace NCL {
	class LoadingMangerBase{
	protected:

		virtual void DisplayLoadingScreen() = 0;		

		LoadingMangerBase(Window* w) {
			loadingComplete = false;
			window = w;
		}
		~LoadingMangerBase() {
		}
	public:
		virtual void Load(std::function<void()> func) = 0;
	protected:
		bool loadingComplete;
		Window* window;
	};
}