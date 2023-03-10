
#include "Window.h"
#include <functional>
#include <thread>
#include <chrono>
#include <iostream>

namespace NCL {
	class LoadingMangerBase{
	public:
		virtual ~LoadingMangerBase() {};
		virtual void Load(std::function<void()> func) = 0;
	protected:

		virtual void DisplayLoadingScreen() = 0;		

		LoadingMangerBase(Window* w) {
			loadingComplete = false;
			window = w;
		}	
	protected:
		bool loadingComplete;
		Window* window;
	};
}