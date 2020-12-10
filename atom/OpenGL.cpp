#include "OpenGL.h"
using namespace Glut::OpenGL;

int main(int argc, char **argv) {
	srand(time(NULL));
	printf("==================== LOG ====================\n\n");
	engine->Start();
	GlutMain(argc, argv);
	AtomEngine::ShutDown();
	return 0;
}