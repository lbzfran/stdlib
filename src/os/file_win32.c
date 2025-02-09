
#include "file.h"
#include <Windows.h>

StringData
FileRead(Arena* arena, StringData filename)
{
    StringData res = {0};
    HANDLE file = CreateFileW((WCHAR *)StringLiteral(filename),
                               GENERIC_READ, 0, 0,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               0);

    if (file == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to access file '%s': (%s).\n", StringLiteral(filename), GetLastError());
        return(res);
    }

    memory_index fileSize = GetFileSize(file, 0);
    printf("file size: %llu\n", fileSize);

    uint8 *buf = PushArray(arena, uint8, fileSize);

    if (!ReadFile(file, buf, fileSize, 0, 0))
    {
        fprintf(stderr, "Failed to read file '%s': (%s).\n", StringLiteral(filename), GetLastError());
    }

    CloseHandle(file);
    return(res);
}

bool32
FileWriteList(StringData filename, StringList data)
{
    bool32 res = false;
    HANDLE file = CreateFileW((WCHAR *)StringLiteral(filename),
                                GENERIC_WRITE, 0, 0,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                0);




    CloseHandle(file);
    return(res);
}

bool32 FileWrite(StringData filename, StringData data)
{
    StringNode node = {};
    StringList list = {};
    StringListPush_(&list, data, &node);
    bool32 res = FileWriteList(filename, list);
    return(res);
}

FileProperties
FileReadProperties(StringData filename)
{
    FileProperties res = {0};
    // TODO(liam): do this

    return(res);
}
