#include "VRApp.h"

// Possibly going to use this to combine with a GUI
int main()
{
	VRApp* TheVRApp = new VRApp();
	TheVRApp->run();
	delete(TheVRApp);
}