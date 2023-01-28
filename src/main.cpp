#include "Math/GameTimer.h"

#include "Window/Window.h"
#include "Window/RendererBase.h"

#include "Game/Renderer.h"

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

using namespace Graphics::Application;
using namespace Math;

int main() {
	GameTimer& timer = GameTimer::getTimer();

	Window::generateWindow(false);
	if (!Window::getWindow().initSuccess())
		return -1;

	Renderer renderer = Renderer(Window::getWindow());
	if (!renderer.initSuccess())
		return -1;

	while (Window::getWindow().update(timer.getMS()) && !Window::getKeyboard().isKeyPressed(KeyboardKey::KEYBOARD_ESCAPE)) {
		timer.updateTime();
		//Window::getWindow().setTitle(std::to_string(timer.getMS()).append("ms"));
		renderer.update();
		renderer.render();
		renderer.swapBuffers();
	}

	Window::destroyWindow();
	return 0;
}
