#include <Windows.h>

#ifndef _MY_UI_
#define _MY_UI_

//HWND CreateAndShowUI(HINSTANCE hInstance, LPCWSTR lpWindowName) {

//}

class MYUI {
	HWND theUI;
	RECT UIRect;

public:
	HWND CreateAndShowUI(LPCWSTR lpWindowName) {

	}

	HWND CreateControl(LPCWSTR);

};

#endif