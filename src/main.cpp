#include "log.hpp"
#include "win.hpp"
#include <conio.h>
#include <iostream>

wchar_t className[] = L"StylusBlockInput";

void OnPointerEnter(bool entered, POINTER_INPUT_TYPE pointerType);
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int Exit(int code, bool confirm = true);

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow) {
  // Register window class
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = className;
  if (!RegisterClassEx(&wc)) {
    std::wcout << "Failed to register the window class.\n";
    return Exit(DumpLastError());
  }

  // Create message-only window
  HWND hWnd = CreateWindowEx(
    0,
    className,
    L"",
    WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
    0,
    0,
    0,
    0,
    HWND_MESSAGE,
    0,
    0,
    0);
  if (hWnd == NULL) {
    std::wcout << "Failed to create the input target window.\n";
    return Exit(DumpLastError());
  }

  // Listen for pen input
  if (!RegisterPointerInputTarget(hWnd, PT_PEN)) {
    std::wcout << "Failed to register the window as a pointer input target.\n ";
    return Exit(DumpLastError());
  }

  // Message loop
  MSG msg;
  BOOL bRet;
  while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0) {
    if (bRet == -1) {
      return Exit(DumpLastError());
    }

    if (bRet == 0) {
      break;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return Exit(0, false);
}

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_POINTERDOWN:
    BlockInput(TRUE);
    break;
  case WM_POINTERUP:
    BlockInput(FALSE);
    break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int Exit(int code, bool confirm) {
  BlockInput(FALSE);
  if (confirm) {
    (void)_getch();
  }
  return code;
}
