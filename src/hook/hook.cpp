#include "hook.hpp"
#include "win.hpp"
#include <iostream>
#include <sstream>

#define SHARED __attribute__((section("SHARED"), shared))

HWND SHARED hWndMain = NULL;

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);

HINSTANCE hInstance = NULL;
HHOOK hHook = NULL;

extern "C" {
__declspec(dllexport) BOOL RegisterHook(HWND hWnd) {
  if (hHook != NULL) return FALSE;

  hWndMain = hWnd;
  hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hInstance, 0);

  return TRUE;
}

__declspec(dllexport) BOOL UnregisterHook() {
  if (hHook == NULL) return FALSE;

  UnhookWindowsHookEx(hHook);

  return TRUE;
}
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reason, LPVOID lpReserved) {
  switch (reason) {
  case DLL_PROCESS_ATTACH:
    hInstance = hModule;
    return TRUE;

  case DLL_PROCESS_DETACH:
    UnregisterHook();
    return TRUE;
  }

  return TRUE;
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
  if (code < 0) {
    CallNextHookEx(hHook, code, wParam, lParam);
    return 0;
  }

  MSG *msg = (MSG *)lParam;
  if (msg->message == WM_POINTERENTER || msg->message == WM_POINTERLEAVE) {
    UINT32 pointerId = GET_POINTERID_WPARAM(msg->wParam);
    POINTER_INPUT_TYPE pointerType;
    if (!GetPointerType(pointerId, &pointerType))
      return CallNextHookEx(hHook, code, wParam, lParam);

    if (msg->message == WM_POINTERENTER) {
      PostMessage(hWndMain, UWM_POINER_ENTER, msg->wParam, (LPARAM)pointerType);
    } else {
      PostMessage(hWndMain, UWM_POINER_LEAVE, msg->wParam, (LPARAM)pointerType);
    }
  }
  // GetLastError() could result in error 5 here, because of UIPI

  return CallNextHookEx(hHook, code, wParam, lParam);
}
