#ifndef STRUCT_HPP
#define STRUCT_HPP
//Completely experimental, it may be proper to fail in other machines
#ifdef _ENABLE_THEME
#pragma comment(linker,"\"/manifestdependency:type='win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#endif
#define NOMINMAX
#include <windows.h>
#include <type_traits>
#include <memory>
#include <functional>
#include <vector>
#include "Core.h"
#include <CommCtrl.h>
//forward declarations
struct Event;
using EventCallback = std::function<void(Event)>;
//type definitions
using BaseHandle = HWND;
using DeviceContextHandle = HDC;
using MenuHandle = HMENU;
using IconHandle = HICON;
using BitmapHandle = HBITMAP;
using FontHandle = HFONT;
using InstanceHandle = HINSTANCE;
using WindowStyle = DWORD;
using ButtonStyle = DWORD;
using ListViewStyle = DWORD;
using ClassStyle = UINT;
using ClassName = const wchar_t*;
using ShowCommand  = int;
using ResourceInt = int;
using WindowMessage = UINT;
using ListViewMessage = UINT;
using Byte = BYTE;
template<typename T> using Reference = std::reference_wrapper<T>;
struct WINCAPE_API Event
{
	BaseHandle handle;
	WPARAM wparam;
	LPARAM lparam;
};
struct WINCAPE_API Int2
{
	int x = 0, y = 0;
};
struct WINCAPE_API Rect
{
		Int2 position, size;

};
namespace WinCape
{
	//TODO: use namespaces
	struct WindowStyles
	{
		static constexpr WindowStyle Border = WS_BORDER;
		static constexpr WindowStyle Caption = WS_CAPTION;
		static constexpr WindowStyle Child = WS_CHILD;
		static constexpr WindowStyle ChildWindow = WS_CHILDWINDOW;
		static constexpr WindowStyle ClipChildren = WS_CLIPCHILDREN;
		static constexpr WindowStyle ClipSiblings = WS_CLIPSIBLINGS;
		static constexpr WindowStyle Disabled = WS_DISABLED;
		static constexpr WindowStyle DialogFrame = WS_DLGFRAME;
		static constexpr WindowStyle Group = WS_GROUP;
		static constexpr WindowStyle HorizontalScroll = WS_HSCROLL;
		static constexpr WindowStyle Iconic = WS_ICONIC;
		static constexpr WindowStyle Maximize = WS_MAXIMIZE;
		static constexpr WindowStyle MaximizeBox = WS_MAXIMIZEBOX;
		static constexpr WindowStyle Minimize = WS_MINIMIZE;
		static constexpr WindowStyle MinimizeBox = WS_MINIMIZEBOX;
		static constexpr WindowStyle Overlapped = WS_OVERLAPPED;
		static constexpr WindowStyle OverlappedWindow = WS_OVERLAPPEDWINDOW;
		static constexpr WindowStyle Popup = WS_POPUP;
		static constexpr WindowStyle PopupWindow = WS_POPUPWINDOW;
		static constexpr WindowStyle SizeBox = WS_SIZEBOX;
		static constexpr WindowStyle SystemMenu = WS_SYSMENU;
		static constexpr WindowStyle TabStop = WS_TABSTOP;
		static constexpr WindowStyle ThickFrame = WS_THICKFRAME;
		static constexpr WindowStyle Tiled = WS_TILED;
		static constexpr WindowStyle TiledWindow = WS_TILEDWINDOW;
		static constexpr WindowStyle Visible = WS_VISIBLE;
		static constexpr WindowStyle VerticalScroll = WS_VSCROLL;
	};
	struct ButtonStyles
	{
		static constexpr ButtonStyle State = BS_3STATE;
		static constexpr ButtonStyle AutoState = BS_AUTO3STATE;
		static constexpr ButtonStyle AutoCheckBox = BS_AUTOCHECKBOX;
		static constexpr ButtonStyle AutoRadioButton = BS_AUTORADIOBUTTON;
		static constexpr ButtonStyle BitMap = BS_BITMAP;
		static constexpr ButtonStyle Bottom = BS_BOTTOM;
		static constexpr ButtonStyle Center = BS_CENTER;
		static constexpr ButtonStyle CheckBox = BS_CHECKBOX;
		//static constexpr ButtonStyle CommandLink = BS_COMMANDLINK;
		//static constexpr ButtonStyle DefCommandLink = BS_DEFCOMMANDLINK;
		static constexpr ButtonStyle DefPushButton = BS_DEFPUSHBUTTON;
		//static constexpr ButtonStyle DefSplitButton = BS_DEFSPLITBUTTON;
		static constexpr ButtonStyle GroupBox = BS_GROUPBOX;
		static constexpr ButtonStyle Icon = BS_ICON;
		static constexpr ButtonStyle Flat = BS_FLAT;
		static constexpr ButtonStyle Left = BS_LEFT;
		static constexpr ButtonStyle LeftText = BS_LEFTTEXT;
		static constexpr ButtonStyle MultiLine = BS_MULTILINE;
		static constexpr ButtonStyle Notify = BS_NOTIFY;
		static constexpr ButtonStyle OwnerDraw = BS_OWNERDRAW;
		static constexpr ButtonStyle PushButton = BS_PUSHBUTTON;
		static constexpr ButtonStyle PushLike = BS_PUSHLIKE;
		static constexpr ButtonStyle RadioButton = BS_RADIOBUTTON;
		static constexpr ButtonStyle Right = BS_RIGHT;
		static constexpr ButtonStyle RightButton = BS_RIGHTBUTTON;
		//static constexpr ButtonStyle SplitButton = BS_SPLITBUTTON;
		static constexpr ButtonStyle Text = BS_TEXT;
		static constexpr ButtonStyle Top = BS_TOP;
		static constexpr ButtonStyle TypeMask = BS_TYPEMASK;
		static constexpr ButtonStyle UserButton = BS_USERBUTTON;
		static constexpr ButtonStyle VCenter = BS_VCENTER;
	};

	struct ListViewStyles
	{
		static constexpr ListViewStyle AlignLeft = LVS_ALIGNLEFT;
		static constexpr ListViewStyle AlignMask = LVS_ALIGNMASK;
		static constexpr ListViewStyle AlignTop = LVS_ALIGNTOP;
		static constexpr ListViewStyle AutoArrange = LVS_AUTOARRANGE;
		static constexpr ListViewStyle EditLabels = LVS_EDITLABELS;
		static constexpr ListViewStyle Icon = LVS_ICON;
		static constexpr ListViewStyle List = LVS_LIST;
		static constexpr ListViewStyle NoColumnHeader = LVS_NOCOLUMNHEADER;
		static constexpr ListViewStyle NoLabelWrap = LVS_NOLABELWRAP;
		static constexpr ListViewStyle NoScroll = LVS_NOSCROLL;
		static constexpr ListViewStyle NoSortHeader = LVS_NOSORTHEADER;
		static constexpr ListViewStyle OwnerData = LVS_OWNERDATA;
		static constexpr ListViewStyle OwnerDrawFixed = LVS_OWNERDRAWFIXED;
		static constexpr ListViewStyle Report = LVS_REPORT;
		static constexpr ListViewStyle ShareImageLists = LVS_SHAREIMAGELISTS;
		static constexpr ListViewStyle ShowSelAlways = LVS_SHOWSELALWAYS;
		static constexpr ListViewStyle SignleSel = LVS_SINGLESEL;
		static constexpr ListViewStyle SmallIcon = LVS_SMALLICON;
		static constexpr ListViewStyle SortAscending = LVS_SORTASCENDING;
		static constexpr ListViewStyle SortDescending = LVS_SORTDESCENDING;
		static constexpr ListViewStyle TypeMask = LVS_TYPEMASK;
		static constexpr ListViewStyle TypeStyleMask = LVS_TYPESTYLEMASK;

		struct Extended {
			static constexpr ListViewStyle AutoArrange = LVS_EX_AUTOAUTOARRANGE;
			static constexpr ListViewStyle AutoCheckSelect = LVS_EX_AUTOCHECKSELECT;
			static constexpr ListViewStyle AutoSizeColumns = LVS_EX_AUTOSIZECOLUMNS;
			static constexpr ListViewStyle AutoBorderSelect = LVS_EX_BORDERSELECT;
			static constexpr ListViewStyle Checkboxes = LVS_EX_CHECKBOXES;
			static constexpr ListViewStyle ColumnOverflow = LVS_EX_COLUMNOVERFLOW;
			static constexpr ListViewStyle ColumnSnapPoints = LVS_EX_COLUMNSNAPPOINTS;
			static constexpr ListViewStyle DoubleBuffer = LVS_EX_DOUBLEBUFFER;
			static constexpr ListViewStyle FlatSB = LVS_EX_FLATSB;
			static constexpr ListViewStyle FullRowSelect = LVS_EX_FULLROWSELECT;
			static constexpr ListViewStyle GridLines = LVS_EX_GRIDLINES;
			static constexpr ListViewStyle HeaderDragDrop = LVS_EX_HEADERDRAGDROP;
			static constexpr ListViewStyle HeaderInAllViews = LVS_EX_HEADERINALLVIEWS;
			static constexpr ListViewStyle HideLabels = LVS_EX_HIDELABELS;
			static constexpr ListViewStyle InfoTip = LVS_EX_INFOTIP;
			static constexpr ListViewStyle JustifyColumns = LVS_EX_JUSTIFYCOLUMNS;
			static constexpr ListViewStyle LabelTip = LVS_EX_LABELTIP;
			static constexpr ListViewStyle MultiWorkAreas = LVS_EX_MULTIWORKAREAS;
			static constexpr ListViewStyle OneClickActivate = LVS_EX_ONECLICKACTIVATE;
			static constexpr ListViewStyle Regional = LVS_EX_REGIONAL;
			static constexpr ListViewStyle SimpleSelect = LVS_EX_SIMPLESELECT;
			static constexpr ListViewStyle SingleRow = LVS_EX_SINGLEROW;
			static constexpr ListViewStyle SnapToGrid = LVS_EX_SNAPTOGRID;
			static constexpr ListViewStyle SubItemImages = LVS_EX_SUBITEMIMAGES;
			static constexpr ListViewStyle TrackSelect = LVS_EX_TRACKSELECT;
			static constexpr ListViewStyle TransparentBackground = LVS_EX_TRANSPARENTBKGND;
			static constexpr ListViewStyle TransparentShadowText = LVS_EX_TRANSPARENTSHADOWTEXT;
			static constexpr ListViewStyle TwoClickActivate = LVS_EX_TWOCLICKACTIVATE;
			static constexpr ListViewStyle UnderLineCold = LVS_EX_UNDERLINECOLD;
			static constexpr ListViewStyle UnderLineHot = LVS_EX_UNDERLINEHOT;
		};

	};

	struct ClassStyles
	{
		static constexpr ClassStyle ByteAlignClient = CS_BYTEALIGNCLIENT;
		static constexpr ClassStyle ByteAlignWindow = CS_BYTEALIGNWINDOW;
		static constexpr ClassStyle ClassDeviceContext = CS_CLASSDC;
		static constexpr ClassStyle DoubleClicks = CS_DBLCLKS;
		static constexpr ClassStyle DropShadow = CS_DROPSHADOW;
		static constexpr ClassStyle GlobalClass = CS_GLOBALCLASS;
		static constexpr ClassStyle HorizontalRedraw = CS_HREDRAW;
		static constexpr ClassStyle NoClose = CS_NOCLOSE;
		static constexpr ClassStyle OwnDeviceContext = CS_OWNDC;
		static constexpr ClassStyle ParentDeviceContext = CS_PARENTDC;
		static constexpr ClassStyle SaveBits = CS_SAVEBITS;
		static constexpr ClassStyle VerticalRedraw = CS_VREDRAW;
	};
	struct ShowCommands
	{
		static constexpr ShowCommand ForceMinimize = SW_FORCEMINIMIZE;
		static constexpr ShowCommand Hide = SW_HIDE;
		static constexpr ShowCommand Maximize = SW_MAXIMIZE;
		static constexpr ShowCommand Minimize = SW_MINIMIZE;
		static constexpr ShowCommand Restore = SW_RESTORE;
		static constexpr ShowCommand Show = SW_SHOW;
		static constexpr ShowCommand ShowDefault = SW_SHOWDEFAULT;
		static constexpr ShowCommand ShowMaximized = SW_SHOWMAXIMIZED;
		static constexpr ShowCommand ShowMinimized = SW_SHOWMINIMIZED;
		static constexpr ShowCommand ShowMinNoActive = SW_SHOWMINNOACTIVE;
		static constexpr ShowCommand ShowNA = SW_SHOWNA;
		static constexpr ShowCommand ShowNoActivate = SW_SHOWNOACTIVATE;
		static constexpr ShowCommand ShowNormal = SW_SHOWNORMAL;
	};
	struct WindowMessages
	{
		//Notifications
		static constexpr WindowMessage ActivateApp = WM_ACTIVATEAPP;
		static constexpr WindowMessage CancelMode = WM_CANCELMODE;
		static constexpr WindowMessage ChildActive = WM_CHILDACTIVATE;
		static constexpr WindowMessage Close = WM_CLOSE;
		static constexpr WindowMessage Command = WM_COMMAND;
		static constexpr WindowMessage Compacting = WM_COMPACTING;
		static constexpr WindowMessage Create = WM_CREATE;
		static constexpr WindowMessage Destroy = WM_DESTROY;
		static constexpr WindowMessage DotPerInchChanged = WM_DPICHANGED;
		static constexpr WindowMessage Enable = WM_ENABLE;
		static constexpr WindowMessage EnterSizeMove = WM_ENTERSIZEMOVE;
		static constexpr WindowMessage ExitSizeMove = WM_EXITSIZEMOVE;
		static constexpr WindowMessage GetIcon = WM_GETICON;
		static constexpr WindowMessage GetMinMaxInfo = WM_GETMINMAXINFO;
		static constexpr WindowMessage InputLangChange = WM_INPUTLANGCHANGE;
		static constexpr WindowMessage InputLangChangeRequest = WM_INPUTLANGCHANGEREQUEST;
		static constexpr WindowMessage MenuCommand = WM_MENUCOMMAND;
		static constexpr WindowMessage MenuSelect = WM_MENUSELECT;
		static constexpr WindowMessage Move = WM_MOVE;
		static constexpr WindowMessage Moving = WM_MOVING;
		static constexpr WindowMessage NCActivate = WM_NCACTIVATE;
		static constexpr WindowMessage NCCalSize = WM_NCCALCSIZE;
		static constexpr WindowMessage NCCreate = WM_NCCREATE;
		static constexpr WindowMessage NCDestroy = WM_NCDESTROY;
		static constexpr WindowMessage Notify = WM_NOTIFY;
		static constexpr WindowMessage Null = WM_NULL;
		static constexpr WindowMessage Paint = WM_PAINT;
		static constexpr WindowMessage QueryDragIcon = WM_QUERYDRAGICON;
		static constexpr WindowMessage QueryOpen = WM_QUERYOPEN;
		static constexpr WindowMessage Quit = WM_QUIT;
		static constexpr WindowMessage SetIcon = WM_SETICON;
		static constexpr WindowMessage ShowWindow = WM_SHOWWINDOW;
		static constexpr WindowMessage Size = WM_SIZE;
		static constexpr WindowMessage Sizing = WM_SIZING;
		static constexpr WindowMessage StyleChanged = WM_STYLECHANGED;
		static constexpr WindowMessage StyleChanging = WM_STYLECHANGING;
		static constexpr WindowMessage ThemeChanged = WM_THEMECHANGED;
		static constexpr WindowMessage UserChanged = WM_USERCHANGED;
		static constexpr WindowMessage WindowPositionChanged = WM_WINDOWPOSCHANGED;
		static constexpr WindowMessage WindowPositionChanging = WM_WINDOWPOSCHANGING;
	};

	struct ListViewMessages {
		static constexpr ListViewMessage ItemChanged = LVN_ITEMCHANGED;
	};


	struct ClassNames {
		static constexpr ClassName ListView = WC_LISTVIEW;
		static constexpr ClassName Button = WC_BUTTON;
		static constexpr ClassName ListBox = WC_LISTBOX;
	};
}
#endif // !STRUCT_HPP