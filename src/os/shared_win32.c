
#include "shared.h"
#include <windows.h>
#include <wchar.h>

void* LibOpen(StringData path)
{
    HMODULE res = LoadLibrary(StringLiteral(path));
    if (!res)
    {
        printf("Failed to load DLL.\n");
    }
    return(res);
}

void* LibLoad(void* lib, char* name)
{
    void* res = GetProcAddress(lib, name);
    if (!res)
    {
        printf("Failed to load function.\n");
    }
    return(res);
}

void LibClose(void *lib)
{
    FreeLibrary(lib);
}
