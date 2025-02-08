
#include "base/base.h"
#include "file.h"
#include "os.h"

typedef uint32 (*sum_t)(uint32*, uint32);

int main(void)
{
    Arena arena = {0};
    StringData path = {0};
    StringData path2 = {0};
    StringData path3 = {0};
    // NOTE(liam): make it easier to find files in a directory.
    // For now, the expectation is that the running dir is the
    // root of the git repo, and the shared object is in ./build
    StringNew(&path, "./build/libtest.so");
    StringNew(&path2, "./README.md");
    StringNew(&path3, "./copy_of_README.md");

    /* DLL TEST */
    /*void* lib = LibOpen(path);*/
    /**/
    /*sum_t sum = (sum_t)LibLoad(lib, "sum");*/
    /**/
    /*uint32 v[] = { 8, 8, 8, 8 };*/
    /*printf("8 * 4 = %d\n", sum(v, 4));*/
    /**/
    /* FILE TEST */
    StringData content = FileRead(&arena, path2);

    StringPrint(content);
    printf("size of content: %lu\n", content.size);

    FileWrite(path3, content);
    FileProperties s = FileReadProperties(path3);

    printf("s->perms: %lu\n", s.access);

    /*LibClose(lib);*/
    ArenaFree(&arena);
    return 0;
}
