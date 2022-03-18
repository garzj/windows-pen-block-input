#include "win.hpp"

HHOOK blockKeyboardHook = NULL;
bool blockKeyboard = false;

static LRESULT CALLBACK
BlockKeyboardLLProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode < 0) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
  }

  if (blockKeyboard) {
    return 1;
  }
  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void RegisterBlockKeyboardHook() {
  if (blockKeyboardHook != NULL) return;

  blockKeyboard = false;
  blockKeyboardHook =
    SetWindowsHookExA(WH_KEYBOARD_LL, BlockKeyboardLLProc, NULL, 0);
}

void UnregisterBlockKeyboardHook() {
  if (blockKeyboardHook == NULL) return;

  UnhookWindowsHookEx(blockKeyboardHook);
  blockKeyboardHook = NULL;
  blockKeyboard = false;
}

void BlockKeyboard(bool block) {
  blockKeyboard = block;
}
