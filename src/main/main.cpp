#include "hook.hpp"
#include "win.hpp"
#include <conio.h>
#include <iostream>

wchar_t className[] = L"StylusDisableKeyboard";

int nPointersPen = 0;

void OnPointerEnter(bool entered, POINTER_INPUT_TYPE pointerType) {
  if (pointerType != PT_PEN) return;

  if (entered) {
    nPointersPen++;
    std::wcout << "Pen pointer entered!\n";
  } else {
    nPointersPen--;
    std::wcout << "Pen pointer left!\n";
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

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow) {
  // Create message-only window
  HWND hWnd = NULL;
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = className;
  if (RegisterClassEx(&wc)) {
    hWnd =
      CreateWindowEx(0, className, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
  }
  if (hWnd == NULL) {
    std::wcout << "Failed to create a message-only window.\n";
    (void)_getch();
    return 1;
  }

  // Inject hook DLL
  HMODULE hookDll = LoadLibrary(L"hook.dll");
  if (hookDll == NULL) {
    std::wcout << "Failed to load the hook.dll library.\n";
    (void)_getch();
    return 1;
  }
  HOOK_REGISTER RegisterHook =
    (HOOK_REGISTER)GetProcAddress(hookDll, "RegisterHook");
  HOOK_UNREGISTER UnregisterHook =
    (HOOK_UNREGISTER)GetProcAddress(hookDll, "UnregisterHook");
  if (RegisterHook == NULL || UnregisterHook == NULL) {
    std::wcout << "Failed to locate the exported DLL functions.\n";
    FreeLibrary(hookDll);
    (void)_getch();
    return 1;
  }
  RegisterHook(hWnd);
  std::wcout << "Injected global pointer hook.\n";

  // Message loop
  MSG msg;
  BOOL bRet;
  while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0) {
    if (bRet == -1) {
      std::wcout << "Message error: " << bRet << "\n";
      (void)_getch();
      break;
    } else if (bRet == 0) {
      break;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnregisterHook();
  FreeLibrary(hookDll);

  return 0;
}
