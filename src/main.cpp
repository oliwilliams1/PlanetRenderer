#include "app.h"

int main() {
	App app = App();
	app.Mainloop();
	return 0;
}

#ifdef _WIN32
#include <Windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) { return main(); }
#endif