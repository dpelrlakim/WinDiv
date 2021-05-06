#include <windows.h>
#include <iostream>
#include "RECT_Ext.h"

// Hotkeys: CTRL + ALT + ...
//    0    -->  exit
// [1...5] -->  window position
//    8    -->  debug
//    9    -->  reset slicing

// NOTE: the int variable named "id" used in this code is meant to be one of [0...9] to go well with the ASCII digit chars.
//   --> For any other numbers or chars, functionality may change.

using namespace std;

const string HOTKEY = "Hotkey 'CTRL + ALT + ";
const string REG = "' registered.\n";
const string UNREG = "' unregistered.\n";
const int DEFAULT_CONSOLE_X = 800;
const int DEFAULT_CONSOLE_Y = 500;
const int CORNER_BUFFER = 300;

// For debugging. Not actually used.
void printRect(RECT& r) {
    cout << "RIGHT: " << r.right << endl;
    cout << "LEFT: " << r.left << endl;
    cout << "TOP: " << r.top << endl;
    cout << "BOTTOM: " << r.bottom << endl;
}

// short for "(un)register and message"
void regAndMsg(int id) {
    if (RegisterHotKey(NULL, id, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, '0' + id)) {
        cout << HOTKEY << id << REG;
    }
}
void unRegAndMsg(int id) {
    if (UnregisterHotKey(NULL, id)) {
        cout << HOTKEY << id << UNREG;
    }
}


// sets 'slices' number of hotkeys which will be used to split up the screen later.
int setSlices(int slices) {
    int oldslices = slices;
    cout << "Choose the number of parts to divide the screen into.\n";
    cin >> slices;
    if (slices > 5) slices = 5; // maximum of 5 slices
    if (slices < 2) slices = 2;

    if (slices > oldslices) {
        for (int i = oldslices + 1; i <= slices; i++) {
            regAndMsg(i);
        }
    }
    else {   // unregister unused hotkeys to avoid confusion
        for (int i = oldslices; i > slices; i--) {
            unRegAndMsg(i);
        }
    }

    return slices;
}

// setting non-slicing related hotkeys (9 and 0 for now)
void setInterface(int id) {
    regAndMsg(id);
}

// Detect monitor nearest the foreground window, and return its RECT object
RECT_EXT detectAndScale(RECT_EXT mon) {
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO); // initialize mi by setting size
    GetMonitorInfoW(MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST), &mi);
    mon = mi.rcWork;
    RECT_EXT ret = mon;
    return ret;
}

int main(int argc, TCHAR* argv[]) {
    HWND cWnd = GetConsoleWindow(); // console window
    HWND topMostWindow = GetForegroundWindow();
    RECT_EXT mon; // RECT structure of nearest monitor

    detectAndScale(mon);

    // set console window to reasonable location & size in case it gets changed by one of the hotkeys
    SetWindowPos(cWnd, HWND_TOP, CORNER_BUFFER, CORNER_BUFFER, DEFAULT_CONSOLE_X, DEFAULT_CONSOLE_Y, SWP_SHOWWINDOW);

    ShowWindow(cWnd, 1);       // show console
    int slot;                  // the windows get moved to (slot / slices) portion of the screen
    int slices = setSlices(0); // set the hotkey CTRL + ALT + [1...slices] (user input)
    setInterface(9);
    setInterface(0);
    ShowWindow(cWnd, 0);       // hide console

    MSG msg;                   // used to receive ID of hotkey
    RECT_EXT cliRect, winRect; // cliRect --> without invisible borders   // winRect --> including invisible borders
    POINT ptBorder;            // ptBorder.x --> width of side borders    // ptBorder.y --> width of bottom border

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY)
        {
            slot = msg.wParam;
            if (slot == 0) break;
            if (slot <= 5) {
                topMostWindow = GetForegroundWindow();
                mon = detectAndScale(mon);

                // Client first moved to the first 1/n of the screen, with its entire window size adjusted
                SetWindowPos(
                    topMostWindow,
                    HWND_TOP,
                    mon.left + (mon.width() / slices) * (slot - 1),
                    mon.top,
                    mon.width() / slices,
                    mon.height(),
                    SWP_SHOWWINDOW);

                // Calculate the length of the borders
                GetWindowRect(topMostWindow, &winRect);
                GetClientRect(topMostWindow, &cliRect);
                ptBorder.x = (winRect.width() - cliRect.width()) / 2;
                ptBorder.y = (winRect.height() - cliRect.height()); // no top border

                // Finally set window size to eliminate gaps 
                SetWindowPos(
                    topMostWindow,
                    HWND_TOP,
                    mon.left + (mon.width() / slices) * (slot - 1) - ptBorder.x,
                    mon.top,
                    mon.width() / slices + 2 * ptBorder.x,
                    mon.height() + ptBorder.y,
                    SWP_SHOWWINDOW);

            } else if (slot == 9) { // ask user again for new 'slices' value
                ShowWindow(cWnd, 1);
                SetForegroundWindow(cWnd);
                SetActiveWindow(cWnd);
                slices = setSlices(slices);
                ShowWindow(cWnd, 0);
            }
        }
    }

    // unregister all hotkeys
    for (int i = 0; i <= slices; i++)
        unRegAndMsg(i);
    unRegAndMsg(9);

    return 0;
}
