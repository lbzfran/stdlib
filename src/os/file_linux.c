
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "base/base.h"
#include "file.h"

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

bool32
FileWriteList(StringData filename, StringList data)
{
    bool32 res = true;
    printf("INFO: executing plan!!!!\n");

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
            fprintf(stderr, "FAILED WRITE: written %lu of %lu.\n", sizeWritten, data.size);
            res = false;
        }
        close(file);
    }
    return(res);
}

bool32
FileWrite(StringData filename, StringData data)
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
