#include "Game.h"

using namespace Eigen;
using namespace std;

//#define USE_NVIDIA

#ifdef USE_NVIDIA
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

int main(int argc, char * argv[]) {
#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	glutInit(&argc, argv);
	Game::Instance.init();
	glutMainLoop();
	Game::Instance.destroy();
}