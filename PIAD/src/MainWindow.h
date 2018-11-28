#pragma once
#include <Cape.h>
#include <string>
#include "Duck/Duck.h"

#include <thread>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>

using namespace WinCape;
struct Pixel32 { std::uint8_t b, g, r, offset = 0; };

enum class DrawAction {
	None = 0,
	DrawImage,
	SaveImage,
	RealTimeVideo,
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
	Menu cameraMenu;
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
	const Rect listViewRect{ 400, 0, 400, 380 };

	cv::VideoCapture cap;
	Int2 defaultCapDim{ 400, 300 };
	Int2 capDimentions;
	cv::VideoWriter videoWriter;
	std::wstring saveVideoFileName;
	std::thread liveThread;
	bool isLiveVideoOn = false;
	bool isRecordingOn = false;
	bool frameReady = false;
	const int maxSavedFrames = 300;
	int savedFrames = 0;

	cv::HOGDescriptor hog; //Histograma de Gradientes Orientados
	std::vector<cv::Scalar> colorList;

public:
	void stopAndWaitForThread();
	MainWindow() : WindowFrame(L"Ventana", Rect{ 120, 120, 800, 480 }) {}
private:
	void onCreate() override;
	void renderFilterBatch();
	void onDraw(DeviceContext deviceContext) override;
	void videoCaptureLoop();
	void onItemChecked(Event e);

	void onButtonLimpiarClick(Event e);
	void onButtonEliminarClick(Event e);

	void addFilterToBatch(const FilterElement & element);

	void onFileMenuSelect(Event e);
	void onPointFiltersMenuSelect(Event e);
	void onLocalFiltersMenuSelect(Event e);
	void onGlobalFiltersMenuSelect(Event e);
	void onCustomFiltersMenuSelect(Event e);
	void onCameraMenuSelect(Event e);
	void loadImage();
	void saveImage();
	void snapshot();
	void realTime();
	void record();
	std::wstring getSaveFileName();
	std::wstring getOpenFileName();

	void applyFilterBatch(duck::Image & src, duck::Image& dst);

	void applyFilterBatch();



};