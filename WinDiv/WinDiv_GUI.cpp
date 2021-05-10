#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <iostream>
#include "RECT_Ext.h"
#include "MY_UI.h"

// Hotkeys: CTRL + ALT + ...
//    0    -->  exit
// [1...5] -->  window position
//    8    -->  debug
//    9    -->  reset slicing

// NOTE: the int variable named "id" used in this code is meant to be one of [0...9] to go well with the ASCII digit chars.
//   --> For any other numbers or chars, functionality may change.

using std::cout;
using std::cin;
using std::endl;
using std::string;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

// creates the UI window, makes it visible and places it at an appropriate location.
HWND initialize(HINSTANCE &hInstance, int nCmdShow, RECT_EXT *UIRect) {

    const wchar_t CLASS_NAME[] = L"WinDiv Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;   // ptr to the WindowProc function
    wc.hInstance = hInstance;      // handle to application instance (created & passed by this wWinMain)
    wc.lpszClassName = CLASS_NAME; // name of class

    RegisterClass(&wc);

    // Create the window.
    HWND UIWin = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"WinDiv",                      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Position and Size
        CORNER_BUFFER, CORNER_BUFFER, 400, 600,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (UIWin == NULL) return 0;

    ShowWindow(UIWin, nCmdShow);
    int UIX, UIY, UIT, UIB;
    if (GetWindowRect(UIWin, UIRect)) {
        UIX = UIRect->left;
        UIY = UIRect->right;
        UIT = UIRect->top;
        UIB = UIRect->bottom;
    }
    return UIWin;
}

HWND createButton(HINSTANCE &hInstance, HWND &parent, RECT_EXT &btnDimensions) {
    HWND bt1 = CreateWindowEx(
        0,
        L"BUTTON",
        L"Move Right",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        // Size and position
        btnDimensions.left, btnDimensions.bottom, btnDimensions.width(), btnDimensions.height(),
        parent,       // Parent window    
        NULL,         // Menu
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
        NULL
    );
    return bt1;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    // __________
    // 
    // get monitor info first. Then create a UI window of appropriate size
    // (ex: If      bigger than X, then 15% of screen, 
    //      else if smaller than Y, then 60% of screen. 
    //      else    Otherwise a constant c)
    HWND topMostWindow = GetForegroundWindow(); // use this to get monitor handle
    RECT_EXT mon; // RECT structure of nearest monitor
    RECT_EXT UIRect;

    detectAndScale(mon);

    HWND UIWin = initialize(hInstance, nCmdShow, &UIRect);
    HWND bt1 = createButton(hInstance, UIWin, UIRect);

    // __________

    int slot;                  // the windows get moved to (slot / slices) portion of the screen
    int slices = setSlices(0); // set the hotkey CTRL + ALT + [1...slices] (user input)
    setInterface(8);
    setInterface(9);
    setInterface(0);

    MSG msg;                   // store user input here
    RECT_EXT cliRect, winRect; // cliRect --> without invisible borders   // winRect --> including invisible borders
    POINT ptBorder;            // ptBorder.x --> width of side borders    // ptBorder.y --> width of bottom border

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

            } else if (slot == 8) { // make a window the same size as of the original console window
                SetWindowPos(topMostWindow, HWND_TOP, CORNER_BUFFER, CORNER_BUFFER, DEFAULT_CONSOLE_X, DEFAULT_CONSOLE_Y, SWP_SHOWWINDOW);
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


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
