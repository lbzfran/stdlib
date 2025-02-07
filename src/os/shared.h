
#include "base/base.h"

void* LibOpen(StringData path);
void* LibLoad(void* lib, char* name);
void LibClose(void* lib);
