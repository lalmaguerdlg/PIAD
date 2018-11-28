#pragma once
#include <Cape.h>
#include <string>
#include "Duck/Duck.h"

using namespace WinCape;
struct Pixel32 { std::uint8_t b, g, r, offset = 0; };

enum class DrawAction {
	None = 0,
	DrawImage,
	SaveImage,
};

class MainWindow : public WindowFrame
{
private:

	DrawAction currentAction;

	Menu menu;
	Menu fileMenu;

	Button buttonAgregar;
	Button buttonLimpiar;
	Button buttonEliminar;
	RadioButton radioButtonA;
	RadioButton radioButtonB;
	RadioButton radioButtonC;
	ListView listView;



	Bitmap image;
	duck::Image buffer;
	
	Bitmap histograms{ Int2{400, 126} };
	std::vector<Pixel32> histogramsBuffer;
	const Rect imageRect{ 0, 0, 400, 300 };
	const Rect histogramsRect{ 0, 300, 400, 126 };
	const Rect listViewRect{ 400, 100, 400, 280 };
public:
	MainWindow() : WindowFrame(L"Ventana", Rect{ 120, 120, 800, 480 }) {}
private:
	void onCreate() override;
	void onDraw(DeviceContext deviceContext) override;
	void onItemChecked(Event e);

	void onButtonAClick(Event e);
	void onButtonBClick(Event e);
	void onRadioButtonAClick(Event e);
	void onFileMenuSelect(Event e);
	void loadImage();
	void saveImage();
	std::wstring getSaveFileName();
	std::wstring getOpenFileName();
};