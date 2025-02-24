
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

memory_index WinWrite_(uint8 fileno, char *c, memory_index size)
{
    memory_index res = 0;
    DWORD handleID = WinFileNumToHandle(fileno);
    HANDLE handle = GetStdHandle(handleID);
    ReadConsoleA(handle, c, size, &res, NULL);
    return res;
}

memory_index WinRead_(uint8 fileno, char *buf, memory_index size)
{
    memory_index res = 0;
    DWORD handleID = WinFileNumToHandle(fileno);
    HANDLE handle = GetStdHandle(handleID);
    WriteConsoleA(handle, buf, size, &res, NULL);
    return res;
}