#include <windows.h>
#include <string>
#include <iomanip>
#include <iostream>
#include "noitaProcess.cpp"
#include "player.cpp"

int main()
{
    DWORD noitaProcess = noitaProcess::getNoitaProcess();
    if (!noitaProcess)
    {
        std::cout << "Unable to find " << noitaProcess::noitaExecName << " running" << std::endl;
        return 1;
    }
    HANDLE noitaHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, noitaProcess);
    std::cout << "Handle to " << noitaProcess::noitaExecName << " made." << std::endl;

    Player::NoitaPlayer::Get().setNoitaHandle(noitaHandle);
    Player::NoitaPlayer::Get().setBaseAddress(noitaProcess::getBaseAddress(noitaProcess::noitaExecName, noitaProcess));
    std::cout << std::hex << Player::NoitaPlayer::Get().readDword(0);
}
