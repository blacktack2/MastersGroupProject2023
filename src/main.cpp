#include "Window/Window.h"
#include "Window/Renderer.h"

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

using namespace Graphics::Application;

int main() {
	Window::generateWindow(false);
	if (!Window::getWindow().initSuccess())
		return -1;

	Graphics::Renderer renderer = Graphics::Renderer(Window::getWindow());
	if (!renderer.initSuccess())
		return -1;

	while (Window::getWindow().update() || Window::getKeyboard().isKeyPressed(KeyboardKey::KEYBOARD_ESCAPE)) {
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.swapBuffers();
	}

	Window::destroyWindow();
	return 0;
}
