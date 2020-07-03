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
    Player::NoitaPlayer::Get().setBaseAddress(noitaProcess::getNoitaBaseAddress(noitaProcess));
    std::cout << std::dec << Player::NoitaPlayer::Get().getGold();
}
