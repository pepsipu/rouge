#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iomanip>
#include <iostream>

namespace noitaProcess
{
    const char noitaExecName[] = "noita.exe";

    uintptr_t getBaseAddress(const char *moduleName, DWORD procId)
    {
        uintptr_t modBaseAddr = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
        if (hSnap != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32 modEntry;
            memset(&modEntry, 0, sizeof(PROCESSENTRY32));
            modEntry.dwSize = sizeof(modEntry);
            if (Module32First(hSnap, &modEntry))
            {
                do
                {
                    if (!stricmp((char *)modEntry.szModule, moduleName))
                    {
                        modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                        break;
                    }
                } while (Module32Next(hSnap, &modEntry));
            }
        }
        CloseHandle(hSnap);
        return modBaseAddr;
    }

    DWORD getNoitaProcess()
    {
        PROCESSENTRY32 process;
        memset(&process, 0, sizeof(PROCESSENTRY32));
        process.dwSize = sizeof(PROCESSENTRY32);

        HANDLE thSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (Process32First(thSnapshot, &process))
        {
            do
            {
                if (!stricmp((char *)process.szExeFile, noitaExecName))
                {
                    CloseHandle(thSnapshot);
                    return process.th32ProcessID;
                }
            } while (Process32Next(thSnapshot, &process));
        }
        CloseHandle(thSnapshot);
        return 0;
    }
} // namespace noitaProcess