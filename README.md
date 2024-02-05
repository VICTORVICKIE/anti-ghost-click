# Anti Ghost Click 

A Simple Win32 System Tray App to prevent ghost clicks in old mouse.

## Description

A Simple System Tray App designed to prevent ghost mouse clicks by implementing a time threshold for consecutive left mouse button clicks.\
The application uses a low-level mouse hook `WH_MOUSE_LL` to intercept mouse events system-wide.\
The LowLevelMouseProc function checks the time interval between consecutive left mouse button clicks and
skips the click if the interval is below a defined threshold based on average clicks per second. 
```c
#define CLICK_INTERVAL_THRESHOLD_MS 150
```
The graphical user interface is minimal, with a system tray icon.\
Right-clicking on the system tray icon opens a context menu with the option to exit the application.

## Getting Started

### Dependencies

* window.h

<!-- ### Installing

* How/where to download your program
* Any modifications needed to be made to files/folders -->

### Compiling the program

 bullets
```shell
gcc -O3 -Wall -Wextra -mwindows .\main.c -o anti-ghost-click
```

## License

This project is licensed under the MIT License - see the LICENSE file for details
