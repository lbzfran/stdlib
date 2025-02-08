
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "base/base.h"
#include "file.h"

StringData FileRead(Arena *arena, StringData filename)
{
    /*ArenaTemp tmp = ArenaScratchCreate(arena);*/
    StringData res = {0};

    FILE *file = fopen((char*)StringLiteral(filename), "r");
    open((char *)filename, O_RDONLY);
    if (!file)
    {
        // TODO(liam): handle err.
        fprintf(stderr, "Failed to read file: %s\n", StringLiteral(filename));
        return(res);
    }


    fseek(file, 0L, SEEK_END);
    memory_index fileSize = ftell(file);
    rewind(file);
    /*memory_index fileSize = fsize(file);*/

    uint8 *buf = PushArray(arena, uint8, fileSize);

    fread(buf, 1, fileSize, file);
    StringNew(&res, buf);

    fclose(file);
    /*ArenaScratchFree(tmp);*/
    return(res);
}

bool32 FileWriteList(StringData filename, StringList data)
{
    bool32 res = true;

    FILE *file = fopen((char*)StringLiteral(filename), "w");
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
    struct stat sb;

    stat((char*)StringLiteral(filename), &sb);

    res.size = sb.st_size;
    res.access = sb.st_mode;
    res.time_created = sb.st_ctime;
    res.time_modified = sb.st_mtime;
    res.flags = S_ISDIR(sb.st_mode);

    return(res);
}
