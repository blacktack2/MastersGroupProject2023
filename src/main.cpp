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

using namespace paintHell;
using namespace core;
using namespace graphics;
using namespace system;
using namespace maths;

static debug::Logger& logger = debug::Logger::getLogger();

void CreateConsole() {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

int main() {
	CreateConsole();

	if (!logger.initSuccess())
		return -1;
	logger.info("Initialized logger.");

	GameTimer& timer = GameTimer::getTimer();
	logger.info("Initialized game timer.");

	Window::createWindow(false);
	if (!Window::getWindow().initSuccess()) {
		logger.fatal("Failed to initialize window", __FILE__, __LINE__);
		return -1;
	}
	logger.info("Initialized main window.");

	Renderer renderer = Renderer();
	if (!renderer.initSuccess()) {
		logger.fatal("Failed to initialize renderer", __FILE__, __LINE__);
		return -1;
	}

	logger.info("Initialized main renderer.");

	renderer.setVSync(VSyncState::ON);

	logger.info("Primary initialization successful. Application starting normally.");

	while (Window::getWindow().update(timer.getMS()) && !Window::getKeyboard().isKeyPressed(KeyboardKey::KEYBOARD_ESCAPE)) {
		timer.updateTime();
		Window::getWindow().setTitle(std::to_string(timer.getMS()).append("ms"));
		renderer.update();
		renderer.render();
		renderer.swapBuffers();
	}

	Window::destroyWindow();

	logger.info("Deconstruction successful. Application exiting normally.");
	return 0;
}
