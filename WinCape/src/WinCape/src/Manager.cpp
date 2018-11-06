#include "../include/Manager.hpp"
#include <tchar.h>
#include <map>
using namespace std;
namespace WinCape
{
	//-------------------------------------------------------------------------
	//Implementation
	//-------------------------------------------------------------------------
	class ManagerImpl
	{
		using EventKey = pair<Base::Handle, WindowMessage>;
		//Wrap in a class?
		using EventMap = map<EventKey, EventCallback>;
		EventMap eventMap;
		//Wrap in font class?
		FontHandle applicationFont = 0;
	public:
		using EventKey = pair<Base::Handle, WindowMessage>;
		//Wrap in a class?
		using EventMap = map<EventKey, EventCallback>;
		static ManagerImpl& instance()
		{
			static ManagerImpl instance_;
			return instance_;
		}
		int startListening()
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return static_cast<int>(msg.wParam);
		}
		void listenEvent(const Base::Handle& handle, const WindowMessage& message, const EventCallback& callback)
		{
			eventMap[EventKey{ handle, message }] = callback;
		}
		void doCallback(const EventKey& key, WPARAM wParam, LPARAM lParam)
		{
			auto keyIterator = eventMap.find(key);
			if (keyIterator != eventMap.end())
			{
				keyIterator->second(Event{ key.first , wParam, lParam });
			}
		}
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto handle = (Base::Handle)hWnd;
			switch (message)
			{
			case WindowMessages::Destroy:
				DeleteObject(ManagerImpl::instance().applicationFont);
				PostQuitMessage(0);
				break;
			case WindowMessages::Command:
			{
				Base::Handle commandHandle = (Base::Handle)lParam;
				WindowMessage controlMessage = HIWORD(wParam);
				ManagerImpl::instance().doCallback(EventKey{ commandHandle, controlMessage }, wParam, lParam);
			}
			break;
			case WindowMessages::MenuCommand:
			{
				Base::Handle commandHandle = (Base::Handle)lParam;
				ManagerImpl::instance().doCallback(EventKey{ commandHandle, message }, wParam, lParam);
			}
			break;
			default:
			{
				ManagerImpl::instance().doCallback(EventKey{ handle, message }, wParam, lParam);
			}
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}
			return 0;
		}
		void registerClass()
		{
			//TODO wrap IDI macros in default header...
			WNDCLASSEX windowClass = {};
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = Defaults::DefClassStyle;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = Application::instance();
			windowClass.hIcon = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
			windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			windowClass.lpszClassName = Defaults::WindowName;
			windowClass.hIconSm = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
			RegisterClassEx(&windowClass);
			//return windowClass;
		}
		Base::Handle createHandle(const wchar_t* className, const wchar_t* text, const WindowStyle& style, const Rect& rect, const Base::Handle& parent)
		{
			auto handle = CreateWindow(
				className,
				text,
				style,
				rect.position.x, rect.position.y,
				rect.size.x, rect.size.y,
				parent,
				NULL,
				Application::instance(),
				NULL
			);
			SendMessage(handle, WM_SETFONT, (WPARAM)Manager::instance().defaultFont(), (LPARAM)MAKELONG(TRUE, 0));
			return handle;
		}
		//Use font wrapper class...
		void defaultFont(const wchar_t* fontName)
		{
			//http://www.cplusplus.com/forum/windows/109795/
			DeleteObject(applicationFont);
			LOGFONT logFont = {};
			logFont.lfHeight = 16;
			_tcscpy_s(logFont.lfFaceName, fontName);
			applicationFont = CreateFontIndirect(&logFont);
		}
		FontHandle defaultFont()
		{
			return applicationFont;
		}
	};
	//-------------------------------------------------------------------------
	//Forwarding
	//-------------------------------------------------------------------------
	Manager::Manager() {}
	Manager& Manager::instance()
	{
		static Manager instance_;
		return instance_;
	}
	int Manager::startListening()
	{
		return ManagerImpl::instance().startListening();
	}
	void Manager::listenEvent(const Base::Handle& handle, const WindowMessage& message, const EventCallback& callback)
	{
		ManagerImpl::instance().listenEvent(handle, message, callback);
	}
	void Manager::registerClass()
	{
		ManagerImpl::instance().registerClass();
	}
	Base::Handle Manager::createHandle(const wchar_t* className, const wchar_t* text, const WindowStyle& style, const Rect& rect, const Base::Handle& parent)
	{
		return ManagerImpl::instance().createHandle(className, text, style, rect, parent);
	}
	void Manager::defaultFont(const wchar_t* fontName)
	{
		ManagerImpl::instance().defaultFont(fontName);
	}
	FontHandle Manager::defaultFont()
	{
		return ManagerImpl::instance().defaultFont();
	}
}