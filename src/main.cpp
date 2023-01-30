#include "Game/Core/TestGameObject.h"
#include <iostream>

#include "Math/GameTimer.h"

#include "Window/Window.h"
#include "Window/RendererBase.h"

#include "Game/Renderer.h"
#include <glm/gtx/string_cast.hpp>

#include "Debugging/Logger.h"

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

using namespace Graphics::Application;
using namespace Math;

void CreateConsole() {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

int main() {
	CreateConsole();

	if (!Debug::Logger::getLogger().initSuccess())
		return -1;
	Debug::Logger::getLogger().info("Initialized logger.");

	GameTimer& timer = GameTimer::getTimer();
	Debug::Logger::getLogger().info("Initialized game timer.");

	Window::createWindow(false);
	if (!Window::getWindow().initSuccess()) {
		Debug::Logger::getLogger().fatal("Failed to initialize window", __FILE__, __LINE__);
		return -1;
	}
	Debug::Logger::getLogger().info("Initialized main window.");

	Renderer renderer = Renderer();
	if (!renderer.initSuccess()) {
		Debug::Logger::getLogger().fatal("Failed to initialize renderer", __FILE__, __LINE__);
		return -1;
	}

	Debug::Logger::getLogger().info("Initialized main renderer.");

	renderer.setVSync(Graphics::VSyncState::ON);

	Debug::Logger::getLogger().info("Primary initialization successful. Application starting normally.");

	while (Window::getWindow().update(timer.getMS()) && !Window::getKeyboard().isKeyPressed(KeyboardKey::KEYBOARD_ESCAPE)) {
		timer.updateTime();
		Window::getWindow().setTitle(std::to_string(timer.getMS()).append("ms"));
		renderer.update();
		renderer.render();
		renderer.swapBuffers();
	}

	Window::destroyWindow();

	Debug::Logger::getLogger().info("Deconstruction successful. Application exiting normally.");
	return 0;
}
