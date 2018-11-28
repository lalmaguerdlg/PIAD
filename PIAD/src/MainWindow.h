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

struct FilterElement {
	duck::Filter filter;
	double sigma = 0;
	bool enabled = true;
	duck::Kernel kernel = duck::Kernel::Identity();
	bool useKernel = false;
	int initCount = 0;
};

class MainWindow : public WindowFrame
{
private:

	DrawAction drawAction;

	Menu menu;
	Menu fileMenu;
	Menu filtersMenu;
	Menu pointFiltersMenu;
	Menu localFiltersMenu;
	Menu globalFiltersMenu;
	Menu customFiltersMenu;

	Button buttonLimpiar;
	Button buttonEliminar;
	ListView listView;

	std::vector<FilterElement> filterBatch;

	Bitmap image;
	duck::Image originalBuffer;
	duck::Image filteredBuffer;
	
	Bitmap histograms{ Int2{256, 256} };
	std::vector<Pixel32> histogramsBuffer;
	const Rect imageRect{ 0, 0, 400, 300 };
	const Rect histogramsRect{ 0, 300, 400, 122 };
	const Rect listViewRect{ 400, 100, 400, 280 };
public:
	MainWindow() : WindowFrame(L"Ventana", Rect{ 120, 120, 800, 480 }) {}
private:
	void onCreate() override;
	void renderFilterBatch();
	void onDraw(DeviceContext deviceContext) override;
	void onItemChecked(Event e);

	void onButtonLimpiarClick(Event e);
	void onButtonEliminarClick(Event e);

	void addFilterToBatch(const FilterElement & element);

	void onFileMenuSelect(Event e);
	void onPointFiltersMenuSelect(Event e);
	void onLocalFiltersMenuSelect(Event e);
	void onGlobalFiltersMenuSelect(Event e);
	void onCustomFiltersMenuSelect(Event e);
	void loadImage();
	void saveImage();
	std::wstring getSaveFileName();
	std::wstring getOpenFileName();

	void applyFilterBatch(duck::Image & src, duck::Image& dst);

	void applyFilterBatch();



};