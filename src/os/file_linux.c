
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "base/base.h"
#include "file.h"


StringData
FileRead(Arena *arena, StringData filename)
{
    StringData res = {0};

    int file = open((char *)StringLiteral(filename), O_RDONLY);
    if (file == -1)
    {
        // TODO(liam): handle err.
        perror("Failed to open file");
        return(res);
    }


    memory_index fileSize = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    uint8 *buf = PushArray(arena, uint8, fileSize);

    if (read(file, buf, fileSize) == -1)
    {
        perror("Failed to read file");
        return(res);
    }
    StringNew(&res, buf);

    close(file);
    return(res);
}


bool32
FileWriteList(Arena *arena, StringData filename, StringList data)
{
    bool32 res = true;

    int file = open((char *)StringLiteral(filename), O_WRONLY | O_CREAT,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (!file)
    {
        res = false;
    }
    else
    {
        // TODO(liam): it may be possible to map this using StringListMap();
        ssize_t sizeWritten = 0;
        if (data.first == data.last)
        {
            if (data.first)
            {
                sizeWritten += write(file, data.first->str.buf, data.first->str.size);
            }
        }
        else
        {
            for (StringNode *current = data.first;
                    current != NULL;
                    current = current->next)
            {
                sizeWritten += write(file, current->str.buf, current->str.size);
            }
        }

        if (sizeWritten != data.size)
        {
            // NOTE(liam): weird impl bc we're letting the write run over and
            // over and only checking after the fact.
            perror("Failed to write file");
            res = false;
        }
        close(file);
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
    struct stat sb;

    bool32 err = stat((char*)StringLiteral(filename), &sb);
    if (err == -1)
    {
        perror("Failed to read file properties");
    }


    res.size = sb.st_size;
    res.access = sb.st_mode;
    res.time_created = sb.st_ctime;
    res.time_modified = sb.st_mtime;
    res.flags = S_ISDIR(sb.st_mode);

    return(res);
}

bool32
FileDelete(Arena *arena, StringData filename)
{
    bool32 res = remove((char *)StringLiteral(filename));
    if (res == -1)
    {
        perror("Failed to delete file");
    }
    return(res);
}

bool32
FileRename(Arena *arena, StringData oldfn, StringData newfn)
{
    // NOTE(liam): Moving a file across filesystems is a no-go,
    // and I will not be adding support for it since I don't specifically
    // need it right now.
    bool32 res = rename((char *)StringLiteral(oldfn), (char *)StringLiteral(newfn));
    if (res == -1)
    {
        perror("Failed to rename file");
    }
    return(res);
}

bool32
FileMakeDirectory(Arena *arena, StringData filename)
{

    bool32 res = mkdir((char *)StringLiteral(filename), 0755);
    if (res == -1)
    {
        perror("Failed to make a new directory");
    }
    return(res);
}

bool32
FileDeleteDirectory(Arena *arena, StringData dirname)
{
    bool32 res = rmdir((char *)StringLiteral(dirname));
    if (res == -1)
    {
        perror("Failed to delete directory");
    }
    return(res);
}

FileIterator
FileIterStart(StringData path)
{
    FileIterator res = {0};
    res.path = path;
    DIR *dr = opendir((char *)StringLiteral((path)));
    if (dr == NULL)
    {
        perror("Failed to iterate on path");
    }
    else
    {
        res.handle = dr;
    }
    return(res);
}

bool32
FileIterNext(Arena *arena, FileIterator *iter, StringData *name)
{
    bool32 res = true;

    iter->entry = readdir(iter->handle);
    if (iter->entry == NULL)
    {
        res = false;
    }
    else
    {
        uint8 *filename = (uint8 *)iter->entry->d_name;
        bool32 isDot = (filename[0] == '.' && filename[1] == 0);
        bool32 isDotDot = (filename[0] == '.' && filename[1] == '.' && filename[2] == 0);

        if (isDot || isDotDot)
        {
            /*printf("INFO: skipping to next file.\n");*/
            res = FileIterNext(arena, iter, name);
        }
        else
        {
            // NOTE(liam): used to also give file properties,
            // but filename received from dirent is not a full path,
            // so it's a lot of work that is pretty much outside
            // the scope of this function.
            StringNew(name, filename);
        }
    }
    return(res);
}

void
FileIterEnd(FileIterator iter)
{
    bool32 res = closedir((DIR *)iter.handle);
    if (res == -1)
    {
        perror("Failed to close directory");
    }
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
