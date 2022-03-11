#include "hook.hpp"
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
  HWND hWnd =
    CreateWindowEx(0, className, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
  if (hWnd == NULL) {
    std::wcout << "Failed to create a message-only window.\n";
    return Exit(DumpLastError());
  }

  // Inject hook DLL
  HMODULE hookDll = LoadLibrary(L"hook.dll");
  if (hookDll == NULL) {
    std::wcout << "Failed to load the hook.dll library.\n";
    return Exit(DumpLastError());
  }
  HOOK_REGISTER RegisterHook =
    (HOOK_REGISTER)GetProcAddress(hookDll, "RegisterHook");
  HOOK_UNREGISTER UnregisterHook =
    (HOOK_UNREGISTER)GetProcAddress(hookDll, "UnregisterHook");
  if (RegisterHook == NULL || UnregisterHook == NULL) {
    FreeLibrary(hookDll);
    std::wcout << "Failed to locate the exported DLL functions.\n";
    return Exit(1);
  }
  RegisterHook(hWnd);
  std::wcout << "Injected global pointer hook.\n";

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

  UnregisterHook();
  FreeLibrary(hookDll);

  return Exit(0, false);
}

int nPointersPen = 0;

void OnPointerEnter(bool entered, POINTER_INPUT_TYPE pointerType) {
  if (pointerType != PT_PEN) return;

  if (entered) {
    nPointersPen++;
    std::wcout << "Pen pointer entered!\n";
    BlockInput(TRUE);
  } else {
    nPointersPen--;
    std::wcout << "Pen pointer left!\n";
    BlockInput(FALSE);
  }
}

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case UWM_POINER_ENTER:
    OnPointerEnter(true, (POINTER_INPUT_TYPE)lParam);
    break;
  case UWM_POINER_LEAVE:
    OnPointerEnter(false, (POINTER_INPUT_TYPE)lParam);
    break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int Exit(int code, bool confirm = true) {
  BlockInput(FALSE);
  if (confirm) {
    (void)_getch();
  }
  return code;
}
