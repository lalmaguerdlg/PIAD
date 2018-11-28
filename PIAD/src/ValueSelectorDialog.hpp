#pragma once
#include <Cape.h>
#include "resource1.h"
#include <string>

#pragma comment(lib, "ComCtl32.lib")
namespace ValueSelectorDialog {
	using namespace WinCape;
	using OnValueSelectedCallback = std::function<void(double)>;
	OnValueSelectedCallback onValueSelected;
	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static Control valueText;
		switch (uMsg)
		{
		case WM_INITDIALOG: {
			valueText.createFromResource(hDlg, IDC_EDIT1);
		}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDCANCEL:
				SendMessage(hDlg, WM_CLOSE, 0, 0);
				return TRUE;
			case IDOK:
				{
					std::wstring text;
					text.resize(256);
					valueText.getText(&text[0], text.size());
					try {
						onValueSelected( std::stod(text) );
						SendMessage(hDlg, WM_CLOSE, 0, 0);
					}
					catch(std::exception e){
						int i = 0;
					}
				}
				return TRUE;
			}
			break;
		case WM_CLOSE:
				EndDialog(hDlg, true);
			return TRUE;
		}

		return FALSE;
	}

	void show(Window::Handle parent, const OnValueSelectedCallback& func) {
		onValueSelected = func;
		DialogBox(Application::instance(), MAKEINTRESOURCE(IDD_DIALOG1), parent, DialogProc);
	}
}