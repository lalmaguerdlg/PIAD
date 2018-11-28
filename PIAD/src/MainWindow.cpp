#include "MainWindow.h"
#include "SaveImage.hpp"


void MainWindow::onCreate() {
	show();
	//Setup window
	Menu::create(menu);
	Menu::create(fileMenu);
	Menu::create(menu);
	Menu::create(fileMenu);
	fileMenu.addItems({
		L"Abrir imagen",
		L"Guardar imagen",
		L"Salir"
	});
	menu.addSubMenu(fileMenu, L"Archivo");
	attachMenu(menu);

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

	listView.addRow(0, { L"", L"Sepia", L"12" });
	listView.setItemChecked(0, true);
	for (int i = 1; i < 20; i++) {
		listView.addRow(i, { L"", L"Gaussiano", L"23" });
		listView.setItemChecked(i, true);
	}

	listView.onItemChecked([&](Event e) { this->onItemChecked(e); });

	//Loading the image...
	image.load(L"W:\\Repos\\PIAD\\PIAD\\src\\Untitled.bmp");
	buffer.resize(image.dimension().x * image.dimension().y);


	//Event listening
	buttonAgregar.onClick([&](Event e) {this->onButtonAClick(e); });
	buttonEliminar.onClick([&](Event e) {this->onButtonBClick(e); });
	radioButtonA.onClick([&](Event e) {this->onRadioButtonAClick(e); });
	fileMenu.onItemSelect([&](Event e) {this->onFileMenuSelect(e); });
	histogramsBuffer.resize(histogramsRect.size.x * histogramsRect.size.y);
	for (auto& pixel : histogramsBuffer)
	{
		pixel.r = 255;
		pixel.g = 0;
		pixel.b = 0;
	}
	histograms.setPixels(&histogramsBuffer[0]);
	srand(GetTickCount());
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

void MainWindow::loadImage() {
	std::wstring filename = getOpenFileName();
	if (filename.length() > 0) {
		image.load(&filename[0]);
		//drawAction = DrawAction::GetLoadedImagePixels;
		//redraw();
	}
}

void MainWindow::saveImage() {
	currentAction = DrawAction::SaveImage;
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

void MainWindow::onDraw(DeviceContext deviceContext) {


	switch (currentAction)
	{
	case DrawAction::None:
		break;
	case DrawAction::DrawImage:
		image.setPixels(&buffer[0]);
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
		currentAction = DrawAction::None;
	} break;
	default:
		break;
	}

	image.clonePixels(&buffer[0]);

	auto randRange = [](const float& min, const float& max) {
		return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
	};

	for (auto& pixel : buffer)
	{
		pixel.r += 25;
	}
	image.setPixels(&buffer[0]);


	/*histograms.clonePixels(&histogramsBuffer[0]);
	for (auto& pixel : histogramsBuffer)
	{
		pixel.r = 255;
		pixel.g = 0;
		pixel.b = 0;
	}
	*/
	deviceContext.drawBitmapStreched(image, imageRect);
	deviceContext.drawBitmapStreched(histograms, histogramsRect);

}
void MainWindow::onItemChecked(Event e) {
	LPNMLISTVIEW pnmv = (LPNMLISTVIEW)e.lparam;
	//Row selected
	if ((pnmv->uChanged & LVIF_STATE)
		&& (pnmv->uNewState & LVIS_SELECTED))
	{
		std::wstring text = std::to_wstring(pnmv->iItem);
		radioButtonA.setText(text.c_str());
	}

	//Checkbox
	if ((pnmv->uChanged & LVIF_STATE)
		&& (pnmv->uNewState & LVIS_STATEIMAGEMASK))
	{
		std::wstring text = std::to_wstring(pnmv->iItem);
		radioButtonA.setText(text.c_str());
	}

}

void MainWindow::onButtonAClick(Event e) {
	minimize();
}
void MainWindow::onButtonBClick(Event e) {
	radioButtonB.setText(L"Poof!");
}
void MainWindow::onRadioButtonAClick(Event e) {
	radioButtonA.setText(L"Me haz clickeado tío!");
	redraw();
}
