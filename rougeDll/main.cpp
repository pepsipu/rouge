#include <windows.h>
#include <cstring>
#include "detours.h"

#pragma comment (lib, "User32.lib")
#pragma comment (lib, "detours.lib")

typedef int (*_luaopen_jit)(void **state);
_luaopen_jit luaopen_jit;



BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID p) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
        HINSTANCE hInst;
        for (hInst = NULL; (hInst = DetourEnumerateModules(hInst)) != NULL;) {
            WCHAR dllPath[MAX_PATH];
            GetModuleFileNameW(hInst, dllPath, sizeof(dllPath));
            WCHAR *dllName;
            if (!(dllName = wcsrchr(dllPath, '\\'))) {
                continue;
            }
            if (!wcscmp(dllName + 1, L"lua51.dll")) {
                break;
            }
        }
        break;
    }
    return true;
}