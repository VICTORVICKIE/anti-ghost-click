// #include <stdio.h>
#include <windows.h>

// Start User Defined
#define CLICK_INTERVAL_THRESHOLD_MS 150
#define APP_NAME "Anti Ghost Click"
#define APP_CLASS "AntiGhostClickClass"
#define APP_ICON "anti-ghost.ico"
// End User Defined

#define IDM_EXIT 1001

HHOOK global_hook;
NOTIFYICONDATA nid;
DWORD last_click_ms = 0;
HMENU hPopupMenu;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN) {
            MSLLHOOKSTRUCT *mouseStruct = (MSLLHOOKSTRUCT *)lParam;
            DWORD curr_click_ms = mouseStruct->time;

            if (last_click_ms) {
                DWORD click_interval_ms = curr_click_ms - last_click_ms;

                if (click_interval_ms <= CLICK_INTERVAL_THRESHOLD_MS) {
                    // printf("if %ld\n", click_interval_ms);
                    return 1; // Skip Click
                } else {
                    // printf("else: %ld\n", click_interval_ms);
                }
            }

            last_click_ms = curr_click_ms;
        }
    }

    return CallNextHookEx(global_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
        nid.uCallbackMessage = WM_USER + 1;
        nid.hIcon =
            (HICON)LoadImage(NULL, APP_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        strcpy(nid.szTip, APP_NAME);

        Shell_NotifyIcon(NIM_ADD, &nid);
    } break;

    case WM_USER + 1: {
        if (LOWORD(lParam) == WM_LBUTTONUP || LOWORD(lParam) == WM_RBUTTONUP) {
            hPopupMenu = CreatePopupMenu();
            AppendMenu(hPopupMenu, MF_STRING, IDM_EXIT, "Exit App");

            POINT cur_pos;
            GetCursorPos(&cur_pos);

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN, cur_pos.x,
                           cur_pos.y, 0, hwnd, NULL);

            PostMessage(hwnd, WM_NULL, 0, 0);
        }
    } break;

    case WM_COMMAND: {
        if (LOWORD(wParam) == IDM_EXIT) {
            PostMessage(hwnd, WM_DESTROY, 0, 0);
        }
    } break;

    case WM_DESTROY: {
        DestroyMenu(hPopupMenu);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
    } break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int main() {
    WNDCLASS window_class = {0};
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.lpszClassName = APP_CLASS;

    RegisterClass(&window_class);

    HWND hwnd = CreateWindowEx(0, APP_CLASS, APP_NAME, 0, 0, 0, 0, 0, 0, 0,
                               GetModuleHandle(NULL), 0);

    if (hwnd == NULL) {
        MessageBox(NULL, "Failed to initialize window!", APP_NAME ": Error",
                   MB_ICONERROR);
        return 1;
    }

    global_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    if (global_hook == NULL) {
        MessageBox(NULL, "Failed to install hook!", APP_NAME ": Error",
                   MB_ICONERROR);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(global_hook);
    CloseWindow(hwnd);
    return 0;
}
