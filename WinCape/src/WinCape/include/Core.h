#pragma once

#ifdef BUILDING_DLL
	#ifdef BUILDING_WINCAPE
	#define WINCAPE_API __declspec(dllexport)
	#else
	#define WINCAPE_API __declspec(dllimport)
	#endif // BUILDING_WINCAPE
#else
	#define WINCAPE_API 
#endif // BUILDING_WINCAPE