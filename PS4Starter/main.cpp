#include <stdlib.h>
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */

#include "../Plugins/PlayStation4/PS4Window.h"
#include "../Plugins/PlayStation4/Ps4AudioSystem.h"
//#include "../Plugins/PlayStation4/PS4Input.h"
#include "../Plugins/PlayStation4/PS4InputManager.h"
#include "TutorialGame.h"

#include <iostream>

#include "ExampleRenderer.h"

using namespace NCL;
using namespace NCL::PS4;

int main(void) {
	PS4Window*w = (PS4Window*)Window::CreateGameWindow("PS4 Example Code", 1920, 1080);

	/*ExampleRenderer renderer(w);*/
	//
	//PS4Input		input		= PS4Input();
	
	PS4::PS4InputManager::Initialize();
	
	/*renderer.CreateCamera(&input);*/

	TutorialGame* game = new TutorialGame();

	Ps4AudioSystem*	audioSystem = new Ps4AudioSystem(8);
	
	GameTimer t;

	while (w->UpdateWindow()) {
		float time = w->GetTimer()->GetTimeDeltaSeconds();

		//input.Poll();
		
		PS4InputManager::Update();
		
		game->UpdateGame(time);

		/*renderer.Update(time);
		renderer.Render();*/
		//Player1
		if (PS4InputManager::GetButtons(PS4::Player1, PS4::CIRCLE)) {
			//std::cout << "Player 1: Circle button pressed" << std::endl;
			return 1;
		}

		//Player2
		if (PS4InputManager::GetButtons(1, 0)) {
			//std::cout << "Player 2:Triangle button pressed" << std::endl;
		}
		if (PS4InputManager::GetButtons(PS4::Player2, PS4::CIRCLE)) {
			//std::cout << "Player 2:Circle button pressed" << std::endl;
			return 1;
		}

	}

	delete audioSystem;
	PS4InputManager::Destroy();
	return 1;
}

