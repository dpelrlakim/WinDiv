//#pragma once ???
#include <Windows.h>

#ifndef _RECT_EXT
#define _RECT_EXT

using namespace std;

struct RECT_EXT : public RECT {
    RECT rec;

    RECT_EXT() : rec{ RECT() } {}
    RECT_EXT(RECT &r) : rec{ RECT(r) } {}
    RECT_EXT operator=(RECT &r) {
        right = r.right;
        left = r.left;
        bottom = r.bottom;
        top = r.top;
        return *this;
    }
    LONG width() const { return right - left; }
    LONG height() const { return bottom - top; }
};

#endif