#include "Game/Core/Transform.h"
#include <iostream>

#include "Math/GameTimer.h"

#include "Window/Window.h"
#include "Window/RendererBase.h"

#include "Game/Renderer.h"
#include <glm/gtx/string_cast.hpp>

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
	std::cout << "hello world\n";
}

int main() {
	GameTimer& timer = GameTimer::getTimer();

	CreateConsole();

	paintHell::core::Transform trans;
	trans.SetLocalPosition(vec3(5, 8, 2));
	trans.DebugLog();
	paintHell::core::Transform trans2;
	trans2.SetLocalPosition(vec3(2, 4, 2));
	trans2.SetParent(&trans);
	trans2.DebugLog();
	std::cout << "World Pos: " << glm::to_string(trans2.GetWorldPosition());

	Window::createWindow(false);
	if (!Window::getWindow().initSuccess())
		return -1;

	Renderer renderer = Renderer();
	if (!renderer.initSuccess())
		return -1;

	renderer.setVSync(Graphics::VSyncState::ON);

	while (Window::getWindow().update(timer.getMS()) && !Window::getKeyboard().isKeyPressed(KeyboardKey::KEYBOARD_ESCAPE)) {
		timer.updateTime();
		Window::getWindow().setTitle(std::to_string(timer.getMS()).append("ms"));
		renderer.update();
		renderer.render();
		renderer.swapBuffers();
	}

	Window::destroyWindow();
	return 0;
}