#include <windows.h>
#include <cstring>
#include <string>
#include <future>
#include <unordered_map>
#include "detours.h"
#include "exports.cpp"

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "detours.lib")

#define DebugPrint(a) MessageBoxA(NULL, (a), "epic", MB_OK)

const char luaScript[] = "rouge.lua";

int fake_luaopen_jit(Exports::lua_State *L)
{
    int ret = Exports::luaopen_jit(L);

    // obtain current handle
    HMODULE hm = NULL;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCSTR)&luaScript, &hm);
    char dllPath[MAX_PATH + sizeof(luaScript)];
    GetModuleFileNameA(hm, dllPath, sizeof(dllPath));
    *(strrchr(dllPath, '\\') + 1) = 0;
    Exports::luaL_loadfilex(L, strcat(dllPath, luaScript), NULL);
    Exports::lua_pcall(L, 0, -1, 0);
    DetourTransactionBegin();
    DetourDetach(&(PVOID &)Exports::luaopen_jit, fake_luaopen_jit);
    DetourTransactionCommit();
    DebugPrint(dllPath);
    return ret;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID p)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        // load all the addresses of the functions we will need
        Exports::EnumerateExports();
        DetourTransactionBegin();
        // give current thread a little heads up we are going to be committing a transaction
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID &)Exports::luaopen_jit, fake_luaopen_jit);
        DetourTransactionCommit();
        break;
    }
    return true;
}