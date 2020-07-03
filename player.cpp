#include <windows.h>

namespace Player
{
    const DWORD playerPointerOffsets[] = {0x11fbb14, 0x6c, 0x2c, 0x260, 0x3c, 0x0};

    enum PlayerOffsets
    {
        Gold = 0x48
    };

    class NoitaPlayer
    {
    public:
        static NoitaPlayer s_Player;
        NoitaPlayer(const NoitaPlayer &) = delete;
        static NoitaPlayer &Get()
        {
            return s_Player;
        }
        void setBaseAddress(DWORD baseAddress)
        {
            this->m_noitaBaseAddress = baseAddress;
            this->m_playerBaseAddress = getPlayerAddress();
        }

        void setNoitaHandle(HANDLE noitaHandle)
        {
            this->m_noitaHandle = noitaHandle;
        }

        DWORD getGold()
        {
            DWORD gold;
            ReadProcessMemory(this->m_noitaHandle, (LPCVOID)(this->m_playerBaseAddress + PlayerOffsets::Gold), &gold, sizeof(DWORD), NULL);
            return gold;
        }
        NoitaPlayer()
        {
        }

    private:
        DWORD getPlayerAddress()
        {
            DWORD lastPointer = this->m_noitaBaseAddress;
            for (int i = 0; i < sizeof(playerPointerOffsets) / sizeof(DWORD); ++i)
            {
                ReadProcessMemory(this->m_noitaHandle, (LPCVOID)(lastPointer + playerPointerOffsets[i]), &lastPointer, sizeof(DWORD), NULL);
            }
            return lastPointer;
        }

        DWORD m_playerBaseAddress;
        DWORD m_noitaBaseAddress;
        HANDLE m_noitaHandle;
    };
    NoitaPlayer NoitaPlayer::s_Player;
} // namespace Player