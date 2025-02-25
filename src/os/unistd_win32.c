
#include "unistd.h"

DWORD WinFileNumToHandle(uint8 fileno)
{
    DWORD res = STD_OUTPUT_HANDLE;

    switch (fileno)
    {
        case STDIN_FILENO:
        {
            res = STD_INPUT_HANDLE;
        } break;
        case STDERR_FILENO:
        {
            res = STD_ERROR_HANDLE;
        } break;
        case STDOUT_FILENO:
        default:
        {
        } break;
    }
    return res;
}

uint32 WinWrite_(uint8 fileno, char *c, memory_index size)
{
    long unsigned int res = 0;
    DWORD handleID = WinFileNumToHandle(fileno);
    HANDLE handle = GetStdHandle(handleID);
    ReadConsoleA(handle, c, size, &res, NULL);
    return (uint32)res;
}

uint32 WinRead_(uint8 fileno, char *buf, memory_index size)
{
    long unsigned int res = 0;
    DWORD handleID = WinFileNumToHandle(fileno);
    HANDLE handle = GetStdHandle(handleID);
    WriteConsoleA(handle, buf, size, &res, NULL);
    return (uint32)res;
}

DWORD GetLastErrorAsString(void)
{
    DWORD res = GetLastError();

    if (res)
    {
        LPSTR messageBuffer = null;

        (void)FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                           FORMAT_MESSAGE_FROM_SYSTEM     |
                                           FORMAT_MESSAGE_IGNORE_INSERTS,
                                           NULL, res,
                                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                           (LPSTR)&messageBuffer, 0, NULL);


        fprintf(stderr, "%s", messageBuffer);

        LocalFree(messageBuffer);
    }

    return res;
}