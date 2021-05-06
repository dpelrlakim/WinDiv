#include <windows.h>
#include <iostream>
#include "RECT_Ext.h"

// Hotkeys: CTRL + ALT + ...
//  0  -->  exit
// 1~5 -->  window position
//  8  -->  debug
//  9  -->  reset slicing

using namespace std;

const string HOTKEY = "Hotkey 'CTRL + ALT + ";
const string REG = "' registered.\n";
const string UNREG = "' unregistered.\n";

const int DEFAULT_CONSOLE_X = 800;
const int DEFAULT_CONSOLE_Y = 500;

int setSlices(int slices) {
    int oldslices = slices;

    cout << "Choose the number of parts to divide the screen into.\n";
    cin >> slices;
    if (slices > 5) slices = 5;
    if (slices < 2) slices = 2;

    if (slices > oldslices) {
        for (int i = oldslices + 1; i <= slices; i++) {
            if (RegisterHotKey(NULL, i, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, '0' + i)) {
                cout << HOTKEY << i << REG;
            }
        }
    }
    else {   // unregister unused hotkeys to avoid confusion
        for (int i = oldslices; i > slices; i--) {
            if (UnregisterHotKey(NULL, i)) {
                cout << HOTKEY << i << UNREG;
            }
        }
    }

    return slices;
}

// NOTE: for hotkeys from CTRL + ALT + [0...9] only
void setInterface(int id) {
    if (RegisterHotKey(NULL, id, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, '0' + id)) {
        cout << HOTKEY << id << REG;
    }
}

// For debugging. Not actually used.
void printRect(RECT& r) {
    cout << "RIGHT: " << r.right << endl;
    cout << "LEFT: " << r.left << endl;
    cout << "TOP: " << r.top << endl;
    cout << "BOTTOM: " << r.bottom << endl;
}

// Detect monitor nearest the foreground window, and return its RECT object
void detectAndScale(RECT_EXT* mon, LONG* h_len, LONG* v_len) {
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO); // initialize mi by setting size
    GetMonitorInfoW(MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST), &mi);
    *mon = mi.rcWork;
    *h_len = mon->width();
    *v_len = mon->height();
}

int main(int argc, TCHAR* argv[]) {
    HWND cWnd = GetConsoleWindow(); // console window
    RECT_EXT mon;
    POINT ptBorder, ptMonitor;

    detectAndScale(&mon, &(ptMonitor.x), &(ptMonitor.y));
    SetWindowPos(cWnd, HWND_TOP, ptMonitor.x / 3, ptMonitor.y / 3, DEFAULT_CONSOLE_X, DEFAULT_CONSOLE_Y, SWP_SHOWWINDOW);

    ShowWindow(cWnd, 1); // 1 - show; 0 - hide
    int slices = setSlices(0);
    setInterface(9);
    setInterface(0);
    ShowWindow(cWnd, 0);

    int slot;

    MSG msg;
    HWND topMostWindow = GetForegroundWindow();
    RECT_EXT cliRect, winRect;

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY)
        {
            slot = msg.wParam;
            if (slot == 0) break;
            if (slot <= 5) {
                topMostWindow = GetForegroundWindow();

                detectAndScale(&mon, &(ptMonitor.x), &(ptMonitor.y));

                // Client first moved to the first 1/n of the screen, with its entire window size adjusted
                SetWindowPos(
                    topMostWindow,
                    HWND_TOP,
                    mon.left + (ptMonitor.x / slices) * (slot - 1),
                    mon.top,
                    mon.width() / slices,
                    mon.height(),
                    SWP_SHOWWINDOW);

                // Calculate the length of the borders
                GetWindowRect(topMostWindow, &winRect);
                GetClientRect(topMostWindow, &cliRect);
                ptBorder.x = (winRect.width() - cliRect.width()) / 2;
                ptBorder.y = (winRect.height() - cliRect.height()); // no border at the top

                // Finally set window size to eliminate gaps 
                SetWindowPos(
                    topMostWindow,
                    HWND_TOP,
                    mon.left + (ptMonitor.x / slices) * (slot - 1) - ptBorder.x,
                    mon.top,
                    ptMonitor.x / slices + 2 * ptBorder.x,
                    ptMonitor.y + ptBorder.y,
                    SWP_SHOWWINDOW);

            }
            else if (slot == 9) { // reset # of slices
                ShowWindow(cWnd, 1);
                SetForegroundWindow(cWnd);
                SetActiveWindow(cWnd); // restore control to console
                slices = setSlices(slices); // take input from user
                ShowWindow(cWnd, 0);
            }
        }
    }

    ShowWindow(cWnd, 1);

    for (int i = 0; i < slices; i++)
        UnregisterHotKey(NULL, i);

    return 0;
}
