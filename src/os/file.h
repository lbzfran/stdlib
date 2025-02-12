#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include "base/string.h"
#include "base/arena.h"

typedef uint32 DataAccessFlags;
enum {
    DataAccessFlags_Read    = (1 << 0),
    DataAccessFlags_Write   = (1 << 1),
    DataAccessFlags_Execute = (1 << 2)
};

typedef uint64 DenseTime;

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

// NOTE(liam): handle casts to DIR* and entry to struct dirent* in LINUX.
// NOTE(liam): make this more generic and cross-platform
typedef struct FileIterator {
    StringData path; // root path
    void *handle;
    struct dirent *entry;
} FileIterator;

StringData FileRead(Arena *arena, StringData filename);
bool32 FileWriteList(Arena *arena, StringData filename, StringList data);
bool32 FileWrite(Arena *arena, StringData filename, StringData data);
FileProperties FileReadProperties(Arena *arena, StringData filename);

bool32 FileDelete(Arena *arena, StringData filename);
bool32 FileRename(Arena *arena, StringData oldfn, StringData newfn);
bool32 FileMakeDirectory(Arena *arena, StringData filename);
bool32 FileDeleteDirectory(Arena *arena, StringData dirname);

FileIterator FileIterStart(StringData path);
bool32 FileIterNext(Arena *arena, FileIterator *iter, StringData *name);
void FileIterEnd(FileIterator iter);

#endif
