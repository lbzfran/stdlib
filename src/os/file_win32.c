
#include "file.h"
#include <Windows.h>

StringData
FileRead(Arena* arena, StringData filename)
{
    StringData res = {0};
    HANDLE file = CreateFileW(StringLiteral(filename),
                               GENERIC_READ, 0, 0,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               0);

    if (file != INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to read file: %s\n", StringLiteral(filename));
        return(res);
    }
    CloseHandle(file);
    return(res);
}

bool32
FileWriteList(StringData filename, StringList data)
{
    bool32 res = false;
    HANDLE file = CreateFileW(StringLiteral(filename),
                                GENERIC_WRITE, 0, 0,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                0);




    CloseHandle(file);
    return(res);
}
bool32 FileWrite(StringData filename, StringData data);
FileProperties FileReadProperties(StringData filename);
