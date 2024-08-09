// Include standard headers
#include "Main.h"

int main(void)
{
	Application* pApp = new Application();
	pApp->Init("", BTX::eBTX_RESOLUTIONS::RES_S_800, false, false);
	pApp->Run();
	SafeDelete(pApp);
	return 0;
}