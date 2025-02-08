
#include "shared.h"
#include <dlfcn.h>

void* LibOpen(StringData path)
{
    void* res = dlopen((char*)StringLiteral(path), RTLD_NOW);
    if (!res)
    {
        fprintf(stderr, "ERROR: dynamic linking error: %s\n", dlerror());
    }

    return(res);
}

void* LibLoad(void* lib, char* name)
{
    void* res = dlsym(lib, name);
    if (!res)
    {
        fprintf(stderr, "ERROR: dynamic linking error: %s\n", dlerror());
    }
    return(res);
}

void LibClose(void *lib)
{
    dlclose(lib);
}

/*int main(void)*/
/*{*/
/*    void* handle = dlopen("libshared.so", RTLD_NOW);*/
/*    void (*hello)(void) = dlsym(handle, "hello");*/
/*    dlclose(handle);*/
/*}*/
