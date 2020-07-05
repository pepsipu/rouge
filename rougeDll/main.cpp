#include <windows.h>
#include <cstring>
#include <future>
#include <unordered_map>
#include <sstream>
#include "detours.h"

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "detours.lib")

typedef int (*luaopen_jit_t)(void **state);
luaopen_jit_t luaopen_jit = NULL;

typedef std::unordered_map<std::string, DWORD *> exportMap_t;
exportMap_t exportsToResolve({
    {"luaopen_jit", (DWORD *)&luaopen_jit},
});
size_t exportsResolved = 0;

std::promise<void> fetchedExports;

BOOL ExportCallback(PVOID programContext, ULONG ordinal, LPCSTR name, PVOID addr)
{
    exportMap_t::const_iterator unresolvedExport = exportsToResolve.find(std::string(name));
    if (unresolvedExport != exportsToResolve.end())
    {
        *(unresolvedExport->second) = (DWORD)addr;
        if (++exportsResolved == exportsToResolve.size())
        {
            fetchedExports.set_value();
            return false;
        }
    }
    return true;
}

void EnumerateExports()
{
    HINSTANCE hInst;
    for (hInst = NULL; (hInst = DetourEnumerateModules(hInst)) != NULL;)
    {
        WCHAR dllPath[MAX_PATH];
        GetModuleFileNameW(hInst, dllPath, sizeof(dllPath));
        WCHAR *dllName;
        if (!(dllName = wcsrchr(dllPath, '\\')))
        {
            continue;
        }
        if (!wcscmp(dllName + 1, L"lua51.dll"))
        {
            break;
        }
    }
    std::future<void> afterExports = fetchedExports.get_future();
    DetourEnumerateExports(hInst, NULL, (PF_DETOUR_ENUMERATE_EXPORT_CALLBACK)ExportCallback);
    // wait for exports to finish
    afterExports.get();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID p)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        // load all the addresses of the functions we will need
        EnumerateExports();
        break;
    }
    return true;
}