#pragma once

#define WINVER 0x602
#define UNICODE
#define _UNICODE

#include <windows.h>

void RegisterBlockKeyboardHook();
void UnregisterBlockKeyboardHook();
void BlockKeyboard(bool block);
