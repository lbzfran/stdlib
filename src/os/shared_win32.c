
#include "shared.h"
#include <Windows.h>

void* LibOpen(StringData path)
{
    void* res = LoadLibraryW((WCHAR*)path.str);
    return(res);
}

void* LibLoad(void* lib, char* name)
{
    void* res = GetProcAddress(lib, name);
    return(res);
}

void LibClose(void* lib)
{
    FreeLibrary(lib);
}
