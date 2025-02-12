#ifndef FILE_H
#define FILE_H

#include "base/string.h"
#include "base/arena.h"
#include "time.h"

typedef uint32 DataAccessFlags;
enum {
    DataAccessFlag_Read    = (1 << 0),
    DataAccessFlag_Write   = (1 << 1),
    DataAccessFlag_Execute = (1 << 2)
};

typedef uint32 FilePropertyFlags;
enum {
    FilePropertyFlag_IsDir = (1 << 0)
};

typedef struct FileProperties {
    memory_index size;
    FilePropertyFlags flags;
    DenseTime time_created;
    DenseTime time_modified;
    DataAccessFlags access;
} FileProperties;

typedef struct FileIterator {
    StringData root; // root path
    void *handle;
} FileIterator;

// NOTE(liam): passing arena is mostly used by win32 api, where
// it uses it to convert the filename's unicode. It might be better to
// convert ahead of time to avoid unnecessary arena passing.
//
// NOTE(liam): FileRead is the only function that actually needs
// nontemporary data to store the file content.
StringData FileRead(Arena *arena, StringData fpath);
bool32 FileWriteList(Arena *arena, StringData fpath, StringList data);
bool32 FileWrite(Arena *arena, StringData fpath, StringData data);
FileProperties FileReadProperties(Arena *arena, StringData fpath);

bool32 FileDelete(Arena *arena, StringData fpath);
bool32 FileRename(Arena *arena, StringData oldfn, StringData newfn);
bool32 FileMakeDirectory(Arena *arena, StringData fpath);
bool32 FileDeleteDirectory(Arena *arena, StringData dpath);

FileIterator FileIterStart(StringData dpath);
bool32 FileIterNext(Arena *arena, FileIterator iter, StringData *dst);
void FileIterEnd(FileIterator iter);

#endif
