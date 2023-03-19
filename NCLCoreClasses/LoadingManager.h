/**
 * @file   LoadingManager.h
 * @brief  Utility class for handling multi-threaded loading of assets.
 * 
 * @author 
 * @date   March 2023
 */
#include "Window.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

namespace NCL {
	/**
	 * @brief Utility class for handling multi-threaded loading of assets.
	 */
	class LoadingMangerBase{
	public:
		virtual ~LoadingMangerBase() {};
		virtual void Load(std::function<void()> func) = 0;
	protected:
		LoadingMangerBase(Window* window) : window(window) {}

		virtual void DisplayLoadingScreen() = 0;		

		Window* window;

		bool loadingComplete = false;
	};
}