
#include "file.h"
#include <stdio.h>
#include <Windows.h>

// NOTE(liam): specific function to windows api.
local StringData
GetLastErrorAsString()
{
    StringData res = {0};

    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0) {
        return(res);
    }

    LPSTR messageBuffer = null;

    memory_index size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                       FORMAT_MESSAGE_FROM_SYSTEM     |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                       NULL, errorMessageID,
                                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                       (LPSTR)&messageBuffer, 0, NULL);

    res.buf = (uint8*)messageBuffer;
    res.size = size;

    LocalFree(messageBuffer);

    return(res);
}


StringData
FileRead(Arena *arena, StringData filename)
{
    StringData res = {0};
    String16Data fn_utf8 = StringConvert16(arena, filename);
    HANDLE file = CreateFile(StringLiteral(fn_utf8),
                               GENERIC_READ, 0, 0,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               0);

    if (file == INVALID_HANDLE_VALUE)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to open file '%s': %s",
                StringLiteral(filename), StringLiteral(errmsg));
        return(res);
    }

    memory_index fileSize = GetFileSize(file, 0);

    uint8 *buf = PushArray(arena, uint8, fileSize);

    if (ReadFile(file, buf, fileSize, 0, 0) == null)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to read file '%s': %s",
                StringLiteral(filename), StringLiteral(errmsg));
        return(res);
    }
    StringNew(&res, buf);

    CloseHandle(file);
    return(res);
}


bool32
FileWriteList(Arena *arena, StringData filename, StringList data)
{
    bool32 res = true;
    String16Data fn_utf8 = StringConvert16(arena, filename);
    HANDLE file = CreateFile(StringLiteral(fn_utf8),
                                GENERIC_WRITE, 0, 0,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                0);
    if (file == INVALID_HANDLE_VALUE)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to open file '%s': %s",
                StringLiteral(filename), StringLiteral(errmsg));
        res = false;
    }
    else
    {
        LPDWORD sizeWritten = 0;
        if (data.first == data.last)
        {
            if (data.first)
            {
                WriteFile(file, data.first->str.buf,
                          data.first->str.size,
                          sizeWritten, 0);
            }
        }
        else
        {
            for (StringNode *current = data.first;
                    current != NULL;
                    current = current->next)
            {
                WriteFile(file, current->str.buf,
                        current->str.size,
                        sizeWritten, 0);
            }
        }

        if ((memory_index)sizeWritten != data.size)
        {
            StringData errmsg = GetLastErrorAsString();
            fprintf(stderr, "Failed to write file '%s': %s",
                    StringLiteral(filename), StringLiteral(errmsg));
            res = false;
        }
        CloseHandle(file);
    }

    return(res);
}

bool32
FileWrite(Arena *arena, StringData filename, StringData data)
{
    StringNode node = {};
    StringList list = {};
    StringListPush_(&list, data, &node);
    bool32 res = FileWriteList(arena, filename, list);
    return(res);
}

FileProperties
FileReadProperties(Arena *arena, StringData filename)
{
    FileProperties res = {0};
    // TODO(liam): do this

    return(res);
}

/***********************/
/*  PORTABLE VERSIONS  */
/***********************/

StringData FileReadPort(Arena *arena, StringData filename)
{
    StringData res = {0};

    FILE *file = fopen((char*)StringLiteral(filename), "r");
    if (!file)
    {
        // TODO(liam): handle err.
        fprintf(stderr, "Failed to read file: %s\n", StringLiteral(filename));
        return(res);
    }

    fseek(file, 0L, SEEK_END);
    memory_index fileSize = ftell(file);
    rewind(file);

    uint8 *buf = PushArray(arena, uint8, fileSize);

    fread(buf, 1, fileSize, file);
    StringNew(&res, buf);

    fclose(file);
    return(res);
}

bool32 FileWriteListPort(StringData filename, StringList data)
{
    bool32 res = true;

    FILE *file = fopen((char *)StringLiteral(filename), "w");
    if (!file)
    {
        res = false;
    }
    else
    {
        StringListPrint_(data, file, '\n');
        fclose(file);
    }
    return(res);
}
