
#include "base/base.h"

#if defined(OS_LINUX)
#define SHARED_LIB "libtest.so"
#elif defined(OS_WINDOWS)
#define SHARED_LIB "test.dll"
#endif

void* LibOpen(StringData path);
void* LibLoad(void* lib, char* name);
void LibClose(void* lib);
