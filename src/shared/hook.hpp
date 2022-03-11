#pragma once

#include "win.hpp"

#define UWM_POINER_ENTER WM_USER + 1
#define UWM_POINER_LEAVE WM_USER + 2

typedef BOOL (*HOOK_REGISTER)(HWND hWnd);
typedef BOOL (*HOOK_UNREGISTER)();
