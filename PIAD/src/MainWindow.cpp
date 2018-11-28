#include "MainWindow.h"
#include "SaveImage.hpp"
#include "ValueSelectorDialog.hpp"


void MainWindow::onCreate() {
	show();
	//Setup window
	Menu::create(menu);
	Menu::create(fileMenu);
	Menu::create(filtersMenu);
	Menu::create(pointFiltersMenu);
	Menu::create(localFiltersMenu);
	Menu::create(globalFiltersMenu);
	Menu::create(customFiltersMenu);
	fileMenu.addItems({
		L"Abrir imagen",
		L"Guardar imagen",
		L"Salir"
	});
	pointFiltersMenu.addItems({
		L"Escala de grises(Promedio)",
		L"Escala de grises(Luminocidad)",
		L"Escala de grises(Luminancia)",
		L"Sepia",
		L"Adición",
		L"Substracción"
	});
	localFiltersMenu.addItems({
		L"Desenfoque(media)",
		L"Afilado",
		L"Desenfoque gaussiano",
		L"Media ponderada",
		L"Substracción de la media",
		L"Sobel horizontal",
		L"Sobel vertical",
		L"Laplaciano",
		L"Mediana"
	});
	globalFiltersMenu.addItems({
		L"Ecualización simple",
		L"Ecualización uniforme",
		L"Ecualización exponencial"
	});
	customFiltersMenu.addItems({
		L"Inversión",
		L"Ecualización binaria"
	});
	filtersMenu.addSubMenu(pointFiltersMenu, L"Filtros puntuales");
	filtersMenu.addSubMenu(localFiltersMenu, L"Filtros locales");
	filtersMenu.addSubMenu(globalFiltersMenu, L"Filtros globales");
	filtersMenu.addSubMenu(customFiltersMenu, L"Filtros extras");

	menu.addSubMenu(fileMenu, L"Archivo");
	menu.addSubMenu(filtersMenu, L"Filtros");
	attachMenu(menu);


	fileMenu.onItemSelect([&](Event e) {this->onFileMenuSelect(e); });
	//editMenu.onItemSelect([&](Event e) {if (imageLoaded) this->onEditMenuSelect(e); });
	//cameraMenu.onItemSelect([&](Event e) {this->onCameraMenuSelect(e); });
	//detectionMenu.onItemSelect([&](Event e) {this->onDetectionMenuSelect(e); });
	pointFiltersMenu.onItemSelect([&](Event e) { this->onPointFiltersMenuSelect(e); });
	localFiltersMenu.onItemSelect([&](Event e) { this->onLocalFiltersMenuSelect(e); });
	globalFiltersMenu.onItemSelect([&](Event e) { this->onGlobalFiltersMenuSelect(e); });
	customFiltersMenu.onItemSelect([&](Event e) { this->onCustomFiltersMenuSelect(e); });


	addButton(buttonLimpiar, L"Limpiar", Int2{ 550, 390 });
	addButton(buttonEliminar, L"Eliminar", Int2{ 700, 390 });

	/*addRadioButton(
		{
			{ radioButtonA, L"radio 1" },
			{ radioButtonB, L"radio 2" },
			{ radioButtonC, L"radio 3" }
		},
		Int2{ 200,200 }
	);*/

	addListView(listView, listViewRect);
	listView.addCheckboxes();
	listView.addColumn(0, (char*)L"Usar", 50);
	listView.addColumn(1, (char*)L"Filtro", 250);
	listView.addColumn(2, (char*)L"Valor", 50);

	listView.onItemChecked([&](Event e) { this->onItemChecked(e); });

	//Loading the image...
	//image.load(L"W:\\Repos\\PIAD\\PIAD\\src\\Untitled.bmp");
	//buffer.resize(image.dimension().x, image.dimension().y);


	//Event listening
	buttonEliminar.onClick([&](Event e) {this->onButtonEliminarClick(e); });
	buttonLimpiar.onClick([&](Event e) {this->onButtonLimpiarClick(e); });
	fileMenu.onItemSelect([&](Event e) {this->onFileMenuSelect(e); });
	histogramsBuffer.resize(256 * 256);
	for (auto& pixel : histogramsBuffer)
	{
		pixel.r = 0;
		pixel.g = 0;
		pixel.b = 0;
	}

	histograms.setPixels(&histogramsBuffer[0]);

	srand(GetTickCount());

	//renderFilterBatch();

}

void MainWindow::renderFilterBatch() {
	int index = 0;
	for (auto& element : filterBatch) {
		listView.addRow(index, { L"", duck::FilterNames[static_cast<int>(element.filter)], std::to_wstring(element.sigma).c_str() });
		listView.setItemChecked(index, element.enabled);
		++index;
	}
}

void MainWindow::onFileMenuSelect(Event e) {
	int index = e.wparam;
	switch (index) {
	case 0:
		loadImage();
		break;
	case 1:
		saveImage();
		break;
	case 2:
		close();
		break;
	}
}

void MainWindow::onPointFiltersMenuSelect(Event e) {
	int index = e.wparam;
	FilterElement element;
	switch (index) {
	case 0:
		element.filter = duck::Filter::PGrayScaleAverage;
		break;
	case 1:
		element.filter = duck::Filter::PGrayScaleLuminosity;
		break;
	case 2:
		element.filter = duck::Filter::PGrayScaleLuminance;
		break;
	case 3:
		element.filter = duck::Filter::PSepia;
		break;
	case 4:
		ValueSelectorDialog::show(handle(), [&](double value) {
			element.filter = duck::Filter::PAddition;
			element.sigma = value;
		});
		break;
	case 5:
		ValueSelectorDialog::show(handle(), [&](double value) {
			element.filter = duck::Filter::PSubstraction;
			element.sigma = value;
		});
		break;
	}
	addFilterToBatch(element);
	drawAction = DrawAction::DrawImage;
	redraw();
}

void MainWindow::onLocalFiltersMenuSelect(Event e) {
	int index = e.wparam;
	FilterElement element;
	element.useKernel = true;
	switch (index) {
	case 0: {
		//Blur
		element.filter = duck::Filter::LBlur;
		element.kernel = duck::Kernel{ 3, 3, { 1, 1, 1, 1, 1, 1, 1, 1, 1} };
	}
			break;
	case 1: {
		//Sharp
		element.filter = duck::Filter::LSharp;
		element.kernel = duck::Kernel{ 3, 3, { 0, -1, 0, -1, 5, -1, 0, -1, 0 } };
	}
			break;
	case 2: {
		//Gaussian
		element.filter = duck::Filter::LGaussian;
		element.kernel = duck::Kernel{ 3, 3,{ 0, 0, 0, 0, 1, 0, 0, 0, 0 } };
		ValueSelectorDialog::show(handle(), [&](double value) {
			element.sigma = value;
			element.kernel = duck::generateGaussian(value);
		});
	}
			break;
	case 3: {
		//media ponderada
		element.filter = duck::Filter::LWeightedAverage;
		element.kernel = duck::Kernel{ 3, 3,{ 0, 0, 0, 0, 1, 0, 0, 0, 0 } };
		ValueSelectorDialog::show(handle(), [&](double value) {
			duck::uchar weight = (duck::uchar)duck::clamp(value, 0, 255);
			element.sigma = weight;
			element.kernel = duck::Kernel{ 3, 3, { 1, 1, 1, 1, weight, 1, 1, 1, 1 } };
		});
	}
			break;
	case 4: {
		//Substraction
		element.filter = duck::Filter::LSubstraction;
		element.kernel = duck::Kernel{ 3, 3,{ -1, -1, -1, -1, 8, -1, -1, -1, -1 }, 1 };
	}
			break;
	case 5: {
		//Sobel X
		element.filter = duck::Filter::LSobelX;
		element.kernel = duck::Kernel{ 3, 3,{ -1, 0, 1, -2, 0, 2, -1, 0, 1 }, 1 };
	}
			break;
	case 6: {
		//Sobel Y
		element.filter = duck::Filter::LSobelY;
		element.kernel = duck::Kernel{ 3, 3,{ 1, 2, 1, 0, 0, 0, -1, -2, -1 }, 1 };
	}
			break;
	case 7: {
		//Laplacian
		element.filter = duck::Filter::LLaplacian;
		element.kernel = duck::Kernel{ 3, 3,{ 0, -1, 0, -1, 4, -1, 0, -1, 0 }, 1 };
	}
			break;
	case 8: {
		element.filter = duck::Filter::LMedian;
		element.useKernel = false;
	}break;
	}
	addFilterToBatch(element);
	drawAction = DrawAction::DrawImage;
	redraw();
}

void MainWindow::onGlobalFiltersMenuSelect(Event e) {
	int index = e.wparam;
	FilterElement element;
	switch (index) {
	case 0: {
		//simple
		element.filter = duck::Filter::GHistogramSimpleEQ;
	}
			break;
	case 1: {
		//uniforme
		element.filter = duck::Filter::GHistogramUniformEQ;
	}
			break;
	case 2: {
		//exponencial
		ValueSelectorDialog::show(handle(), [&](double value) {
			element.filter = duck::Filter::GHistogramExponentialEQ;
			element.sigma = value;
		});
	}
			break;
	}
	addFilterToBatch(element);
	drawAction = DrawAction::DrawImage;
	redraw();
}

void MainWindow::onCustomFiltersMenuSelect(Event e) {
	int index = e.wparam;
	FilterElement element;
	switch (index) {
	case 0:
		element.filter = duck::Filter::CInverse;
		break;
	case 1:
		element.filter = duck::Filter::CBinary;
		break;
	}
	addFilterToBatch(element);
	drawAction = DrawAction::DrawImage;
	redraw();
}

void MainWindow::loadImage() {
	std::wstring filename = getOpenFileName();
	if (filename.length() > 0) {
		image.load(&filename[0]);
		originalBuffer.resize(image.dimension().x, image.dimension().y);

		image.clonePixels(originalBuffer.rawBegin());
		filteredBuffer = originalBuffer;

		drawAction = DrawAction::DrawImage;
		redraw();
	}
}

void MainWindow::saveImage() {
	drawAction = DrawAction::SaveImage;
	redraw();
}

std::wstring MainWindow::getSaveFileName() {
	wchar_t filename[MAX_PATH] = L"";
	OPENFILENAME openFileName = {};
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.hwndOwner = handle();
	openFileName.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp";
	openFileName.lpstrFile = filename;
	openFileName.nMaxFile = sizeof(filename);
	openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	openFileName.lpstrDefExt = (LPCWSTR)L"bmp";
	if (GetSaveFileName(&openFileName)) {
		return std::wstring{ openFileName.lpstrFile };
	}
	return std::wstring{};
}

std::wstring MainWindow::getOpenFileName() {
	wchar_t filename[MAX_PATH] = L"";
	OPENFILENAME openFileName = {};
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.hwndOwner = handle();
	openFileName.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp";
	openFileName.lpstrFile = filename;
	openFileName.nMaxFile = sizeof(filename);
	openFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if (GetOpenFileName(&openFileName)) {
		return std::wstring{ openFileName.lpstrFile };
	}
	return std::wstring{};
}

void MainWindow::applyFilterBatch(duck::Image& src, duck::Image& dst)
{
	using namespace duck;
	dst = src;
	for (auto& element : filterBatch) {
		if (!element.enabled)
			continue;
		switch (element.filter)
		{
		case Filter::PGrayScaleAverage:
			toGrayScaleAverage(dst);
			break;
		case Filter::PGrayScaleLuminance:
			toGrayScaleLuminance(dst);
			break;
		case Filter::PGrayScaleLuminosity:
			toGrayScaleLuminosity(dst);
			break;
		case Filter::PSepia:
			toSepia(dst);
			break;
		case Filter::PAddition:
			add(dst, (uchar)element.sigma);
			break;
		case Filter::PSubstraction:
			substract(dst, (uchar)element.sigma);
			break;
		case Filter::LBlur:
		case Filter::LSharp:
		case Filter::LGaussian:
		case Filter::LWeightedAverage:
		case Filter::LSubstraction:
		case Filter::LSobelX:
		case Filter::LSobelY:
		case Filter::LLaplacian: {
			Image tmp = dst;
			convolve(element.kernel, tmp, dst);
		}	break;
		case Filter::LMedian: {
			Image tmp = dst;
			convolveMiddle(tmp, dst);
		}	break;
		case Filter::GHistogramSimpleEQ:
			histogramSimpleEQ(dst);
			break;
		case Filter::GHistogramUniformEQ:
			histogramUniformEQ(dst);
			break;
		case Filter::GHistogramExponentialEQ:
			histogramExponentialEQ(dst, element.sigma);
			break;
		case Filter::CInverse:
			invert(dst);
			break;
		case Filter::CBinary:
			binary(dst);
			break;
		default:
			break;
		}
	}
}

void MainWindow::onDraw(DeviceContext deviceContext) {


	switch (drawAction)
	{
	case DrawAction::None:
		break;
	case DrawAction::DrawImage:
		if (originalBuffer.width() > 0 && originalBuffer.height() > 0) {
			applyFilterBatch(originalBuffer, filteredBuffer);
			image.setPixels(filteredBuffer.rawBegin());
			
			duck::Image histogramsImage;
			duck::makeHistogram(filteredBuffer, histogramsImage);
			histograms.setPixels(histogramsImage.rawBegin());

		}
		drawAction = DrawAction::None;
		break;
	case DrawAction::SaveImage: {
		std::wstring str = getSaveFileName();
		if (str.length() > 0) {
			CreateBMPFile(
				handle(),
				&str[0],
				CreateBitmapInfoStruct(handle(), image.handle()),
				image.handle(),
				deviceContext.handle()
			);
		}
		drawAction = DrawAction::None;
	} break;
	default:
		break;
	}

	deviceContext.drawBitmapStreched(image, imageRect);
	deviceContext.drawBitmapStreched(histograms, histogramsRect);

}

void MainWindow::onItemChecked(Event e) {
	LPNMLISTVIEW pnmv = (LPNMLISTVIEW)e.lparam;
	//Row selected
	/*if ((pnmv->uChanged & LVIF_STATE)
		&& (pnmv->uNewState & LVIS_SELECTED))
	{

	}*/

	//Checkbox
	if ((pnmv->uChanged & LVIF_STATE)
		&& (pnmv->uNewState & LVIS_STATEIMAGEMASK))
	{
		if (pnmv->iItem >= 0 && pnmv->iItem < filterBatch.size()) {
			if (filterBatch[pnmv->iItem].initCount < 2) {
				++filterBatch[pnmv->iItem].initCount;
			}
			else {
				switch (pnmv->uNewState & LVIS_STATEIMAGEMASK)
				{
				case INDEXTOSTATEIMAGEMASK(2):
					// pnmv->iItem was checked
					filterBatch[pnmv->iItem].enabled = true;
					break;
				case INDEXTOSTATEIMAGEMASK(1):
					//pnmv->iItem was unchecked
					filterBatch[pnmv->iItem].enabled = false;
					break;
				}
				drawAction = DrawAction::DrawImage;
				redraw();
			}
		}

	}

}

void MainWindow::onButtonLimpiarClick(Event e) {
	listView.clear();
	filterBatch.clear();
	drawAction = DrawAction::DrawImage;
	redraw();
}

void MainWindow::onButtonEliminarClick(Event e) {
	int selected = listView.selectedRow();
	if (selected >= 0) {
		listView.clear();
		filterBatch.erase(filterBatch.begin() + selected);
		for (auto& element : filterBatch) {
			element.initCount = 0;
		}
		renderFilterBatch();
		drawAction = DrawAction::DrawImage;
		redraw();
	}
}

void MainWindow::addFilterToBatch(const FilterElement& element) {
	int index = filterBatch.size();
	filterBatch.push_back(element);

	listView.addRow(index, { L"", duck::FilterNames[static_cast<int>(element.filter)], std::to_wstring(element.sigma).c_str() });
	listView.setItemChecked(index, element.enabled);
}
