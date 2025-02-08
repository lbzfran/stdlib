#ifndef FILE_H
#define FILE_H

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

StringData FileRead(Arena* arena, StringData filename);
bool32 FileWriteList(StringData filename, StringList data);
bool32 FileWrite(StringData filename, StringData data);
FileProperties FileReadProperties(StringData filename);

#endif
