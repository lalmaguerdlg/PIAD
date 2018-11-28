#define _ENABLE_THEME
#include "MainWindow.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Setup font
	//TODO... font class wrapper...
	Application::defaultFont(L"Segoe UI");
	//Beginning to run the application...
	//Also add a 'font' param to default controls font style?
	MainWindow window;
	Application::run(window);
}
