#include "NetworkCommon.h"
#include <Windows.h>

namespace NetworkCommon
{
    bool initializeWinsock()
    {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            std::cerr << "WSAStartup failed: " << result << std::endl;
            return false;
        }
        return true;
    }

    void cleanupWinsock()
    {
        WSACleanup();
    }

    std::string getLastErrorAsString()
    {
        DWORD errorMessageID = WSAGetLastError();
        if (errorMessageID == 0)
        {
            return "No error";
        }

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorMessageID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer,
            0,
            NULL);

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }
}