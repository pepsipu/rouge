#include <windows.h>
#include <cstring>
#include <string>
#include <future>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "detours.h"
#include "exports.cpp"

namespace Lua
{
#include "lua5.1/lstate.h"
}

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "detours.lib")

#define DebugPrint(a) MessageBoxA(NULL, (a), "epic", MB_OK)

const char luaScript[] = "rouge.lua";

// int fake_luaopen_jit(Lua::lua_State *L)
// {
//     int ret = Exports::luaopen_jit((Exports::lua_State *)L);

//     // obtain current handle
//     HMODULE hm = NULL;
//     GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
//                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
//                       (LPCSTR)&luaScript, &hm);
//     char dllPath[MAX_PATH + sizeof(luaScript)];
//     GetModuleFileNameA(hm, dllPath, sizeof(dllPath));
//     *(strrchr(dllPath, '\\') + 1) = 0;
//     strcat(dllPath, luaScript);
//     Exports::luaL_loadfilex((Exports::lua_State *)L, dllPath, NULL);
//     Exports::lua_pcall((Exports::lua_State *)L, 0, -1, 0);
//     DetourTransactionBegin();
//     DetourDetach(&(PVOID &)Exports::luaopen_jit, fake_luaopen_jit);
//     DetourTransactionCommit();
//     DebugPrint(dllPath);
//     return ret;
// }

int fake_lua_loadbufferx(Exports::lua_State *L, const char *buf, size_t sz, const char *name, const char *mode)
{
    if (!strcmp(name, "data/scripts/audio/mountain_enter.lua"))
    {
        // everything is in place, time to attack!
        Lua::lua_State *trueSt = (Lua::lua_State *)L;
        // fetch mal lua script
        HMODULE hm = NULL;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR)&luaScript, &hm);
        char dllPath[MAX_PATH + sizeof(luaScript)];
        GetModuleFileNameA(hm, dllPath, sizeof(dllPath));
        *(strrchr(dllPath, '\\') + 1) = 0;
        strcat(dllPath, luaScript);
        std::ifstream luaFile(dllPath);
        std::string luaCode((std::istreambuf_iterator<char>(luaFile)), std::istreambuf_iterator<char>());
        return Exports::luaL_loadbufferx(L, strdup(luaCode.c_str()), strlen(luaCode.c_str()), name, mode);
    }
    return Exports::luaL_loadbufferx(L, buf, sz, name, mode);
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
        // DetourAttach(&(PVOID &)Exports::luaopen_jit, fake_luaopen_jit);
        DetourAttach(&(PVOID &)Exports::luaL_loadbufferx, fake_lua_loadbufferx);
        DetourTransactionCommit();
        break;
    }
    return true;
}