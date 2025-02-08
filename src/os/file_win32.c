
#include "file.h"
#include <window.h>

StringData
FileRead(Arena* arena, StringData filename)
{
    StringData res = {0};
    HANDLE file = CreateFileW((char *)StringLiteral(filename),
                               GENERIC_READ, 0, 0,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               0);



    return(res);
}
bool32 FileWriteList(StringData filename, StringList data);
bool32 FileWrite(StringData filename, StringData data);
FileProperties FileReadProperties(StringData filename);
