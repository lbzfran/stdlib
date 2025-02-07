#ifndef FILE_H
#define FILE_H

#include "base/string.h"
#include "base/arena.h"
String FileRead(Arena* arena, String filename);
bool32 FileWrite(String filename, StringList data);

#endif