//#include <defines.hpp>
#include "WinCape.hpp"
#include "Core.h"
namespace WinCape
{
	class WINCAPE_API Manager
	{
	private:
		Manager();
	public:
		int startListening();
		void listenEvent(const Base::Handle& handle, const WindowMessage& message, const EventCallback& callback);
		void defaultFont(const wchar_t* fontName);
		FontHandle defaultFont();
		void registerClass();
		Base::Handle createHandle(const wchar_t* className, const wchar_t* text, const WindowStyle& style, const Rect& rect, const Base::Handle& parent = NULL);
		static Manager& instance();
	};
}