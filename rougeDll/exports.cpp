#include <windows.h>
#include <cstring>
#include <future>
#include <unordered_map>
#include <sstream>
#include "detours.h"

namespace Exports
{
    typedef std::unordered_map<std::string, DWORD *> exportMap_t;

    typedef void *lua_State;

    typedef int (*_luaL_loadfilex)(lua_State *L, const char *filename, const char *mode);
    _luaL_loadfilex luaL_loadfilex;

    typedef int (*_luaopen_jit)(lua_State *L);
    _luaopen_jit luaopen_jit;

    typedef int (*_lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
    _lua_pcall lua_pcall;

    exportMap_t exportsToResolve({
        {"luaopen_jit", (DWORD *)&luaopen_jit},
        {"luaL_loadfilex", (DWORD *)&luaL_loadfilex},
        {"lua_pcall", (DWORD *)&lua_pcall},
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
} // namespace Exports