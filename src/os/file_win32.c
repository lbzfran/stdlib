
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

// local DateTime
// SysToDateTime(SYSTEMTIME st)
// {
//     DateTime res = {0};
//     res.year = st.wYear;
//     res.month = (uint8)st.wMonth;
//     res.day = st.wDay;
//     res.hour = st.wHour;
//     res.min = st.wMinute;
//     res.sec = st.wSecond;
//     res.ms = st.wMilliseconds;
//     return(res);
// }

// local DenseTime
// FileTimeToDense(FILETIME *ft)
// {
//     SYSTEMTIME systime = {0};
//     FileTimeToSystemTime(ft, &systime);
//     DateTime dt = SysToDateTime(systime);
//     DenseTime res = DateTimeToDense(dt);
//     return(res);
// }

StringData
FileRead(Arena *arena, StringData filename)
{
    StringData res = {0};
    String16Data fn_utf16 = StringConvert16(arena, filename);
    HANDLE file = CreateFileW(StringLiteral(fn_utf16),
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
    String16Data fn_utf16 = StringConvert16(arena, filename);
    HANDLE file = CreateFileW(StringLiteral(fn_utf16),
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
        memory_index sizeWritten = 0;
        DWORD bufSizeWritten = 0; // WriteFile sets this to null on every write.
        if (data.first == data.last)
        {
            if (data.first)
            {
                WriteFile(file, data.first->str.buf,
                          data.first->str.size,
                          &bufSizeWritten, 0);
                sizeWritten = (memory_index)bufSizeWritten;
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
                        &bufSizeWritten, 0);
                sizeWritten += (memory_index)bufSizeWritten;
            }
        }

        if (sizeWritten != data.size)
        {
            StringData errmsg = GetLastErrorAsString();
            fprintf(stderr, "Failed to write file '%s': size %llu of %llu; %s",
                    StringLiteral(filename),
                    sizeWritten,
                    data.size,
                    StringLiteral(errmsg)
            );
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
    String16Data fn_utf16 = StringConvert16(arena, filename);

    WIN32_FILE_ATTRIBUTE_DATA sb = {0};
    if (GetFileAttributesExW(StringLiteral(fn_utf16), GetFileExInfoStandard, &sb))
    {
        res.size = ((uint64)sb.nFileSizeHigh << 32) | (uint64)sb.nFileSizeLow;
        res.flags = (sb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? FilePropertyFlag_IsDir : 0);
        // TODO(liam): add time values when theyre implemented.
        res.time_created = 0;
        res.time_modified = 0;
        res.access = DataAccessFlag_Read | DataAccessFlag_Execute;
        if (!(sb.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
        {
            res.access |= DataAccessFlag_Write;
        }

    }
    return(res);
}

bool32
FileDelete(Arena *arena, StringData filename)
{
    String16Data fn_utf16 = StringConvert16(arena, filename);
    bool32 res = DeleteFileW(StringLiteral(fn_utf16));
    if (res == false)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to delete file '%s': %s",
                StringLiteral(filename), StringLiteral(errmsg));
    }
    return(res);
}

bool32
FileRename(Arena *arena, StringData oldfn, StringData newfn)
{
    String16Data oldfn_utf16 = StringConvert16(arena, oldfn);
    String16Data newfn_utf16 = StringConvert16(arena, newfn);

    bool32 res = MoveFileExW(StringLiteral(oldfn_utf16), StringLiteral(newfn_utf16),
                           MOVEFILE_FAIL_IF_NOT_TRACKABLE);
    if (res == false)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to rename file '%s' to '%s': %s",
                StringLiteral(oldfn), StringLiteral(newfn), StringLiteral(errmsg));
    }
    return(res);

}

bool32
FileMakeDirectory(Arena *arena, StringData dirname)
{
    String16Data fn_utf16 = StringConvert16(arena, dirname);
    bool32 res = CreateDirectoryW(StringLiteral(fn_utf16), 0);
    if (res == false)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to make directory '%s': %s",
        StringLiteral(dirname), StringLiteral(errmsg));
    }
    return(res);
}

bool32
FileDeleteDirectory(Arena *arena, StringData dirname)
{
    String16Data fn_utf16 = StringConvert16(arena, dirname);
    bool32 res = RemoveDirectoryW(StringLiteral(fn_utf16));
    if (res == false)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to delete directory '%s': %s",
                StringLiteral(dirname), StringLiteral(errmsg));
    }
    return(res);
}

FileIterator
FileIterStart(StringData dpath)
{
    FileIterator res = {0};

    StringData sep = {0};
    StringNew(&sep, "\\*");

    StringNode nodes[2];
    StringList list = {0};
    StringListPush_(&list, dpath, nodes);
    StringListPush_(&list, sep, nodes + 1);
    StringData fullpath = StringListJoin(arena, &list, null);

    String16Data fn_utf16 = StringConvert16(arena, fullpath);
    HANDLE *dirhandle = FindFirstFileW(fn_utf16, );
    if (handle == INVALID_HANDLE_VALUE)
    {
        StringData errmsg = GetLastErrorAsString();
        fprintf(stderr, "Failed to make directory '%s': %s",
        StringLiteral(dpath), StringLiteral(errmsg));
    }
    else
    {
        res.root = dpath;
        res.handle = dirhandle;
    }
    return(res);
}

bool32
FileIterNext(Arena *arena, FileIterator iter, StringData *dst)
{

}

void
FileIterEnd(FileIterator iter)
{

}

/***********************/
/*  PORTABLE VERSIONS  */
/***********************/

StringData
FileReadPort(Arena *arena, StringData filename)
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

bool32
FileWriteListPort(StringData filename, StringList data)
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
