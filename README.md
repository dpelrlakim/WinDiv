# WinDiv
WinDiv allows you to use hotkeys to move & resize any window to an equally divided section of the monitor (similar to `Windows Key` + `Arrow Keys`). 

## How to use this program
First, select the number of pieces to divide your screen into. Input the number into the command line and press enter. Let's call this number n. You can choose any n-value from 2 to 5. <br/>
Then, click on a window you want to reposition/resize and use the hotkey `CTRL` + `ALT` + `[1...n]`. You should be able to quickly move around the windows like that.<br/>
For example, if I wanted to move my browser window to the 3rd 1/5 of the screen, I would input 5 in the beginning, then use the hotkey `CTRL` + `ALT` + `3`.

## Extra functions
- `CTRL` + `ALT` + `8` to make a window smaller (same size as the command line window when program is started)
- `CTRL` + `ALT` + `9` to re-select n-value
- `CTRL` + `ALT` + `0` to exit

## Notes
- This application supports multi-display setups! If you have a 2nd monitor, simply drag the window so that more of its area is covered by that monitor. Then, using one of the hotkeys will automatically move it to the correct portion of that monitor.
- This application will automatically hide the command line after receiving the input (number of monitor sections to make) from you. However, it doesn't change focus to a different window for you, so you need to click on the window to bring focus to it before you use one of the hotkeys. You can use `ALT` + `TAB` to bring focus to your most recent window to bypass using a mouse.
- You may find `CTRL` + `ALT` + `8` helpful if you don't want to drag the border of a window from the edge of the screen to make it smaller.

Enjoy!
