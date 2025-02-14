
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
    StringData path4 = {0};
    StringData path5 = {0};
    // NOTE(liam): make it easier to find files in a directory.
    // For now, the expectation is that the running dir is the
    // root of the git repo, and the shared object is in ./build
    StringNew(&path, "./build/" SHARED_LIB);
    StringNew(&path2, "./README.md");
    StringNew(&path3, "./copy_of_README.md");
    StringNew(&path4, "./test/");
    StringNew(&path5, "./newwercopy.md");

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

    //StringPrint(content);
    printf("size of content: %lu\n", content.size);

    FileDelete(&arena, path5);
    //FileDelete(&arena, path3);
    FileWrite(&arena, path3, content);
    FileProperties s = FileReadProperties(&arena, path4);

    printf("path test: ./test/\n");
    printf("s->perms: %lu\n", s.access);
    printf("s->isdir: %d\n", s.flags);

    StringData dirpath1 = {0};
    StringNew(&dirpath1, ".");

    //FileMakeDirectory(&arena, dirpath1);
    //FileDeleteDirectory(&arena, dirpath1);

    // FileDelete(&arena, path3);
    FileRename(&arena, path3, path5);

    FileIterator iter = FileIterStart(&arena, dirpath1);

    printf("rootpath of iter: ");
    StringPrintn(iter.root);
    // StringPrintn(iter.entry);

    while (FileIterNext(&arena, &iter, &path2))
    {
        StringPrintn(path2);
    }

    FileIterEnd(iter);

    /* TIME TEST */

    DateTime uni = TimeUniversal();
    DateTime now = TimeUniversalToLocal(uni);
    printf("Time:\n");
    printf("sec=%u; ms=%u\n", now.sec, now.ms);
    printf("h=%u; min=%u\n", now.hour, now.min);
    printf("%u.%u.%d\n", now.year, now.month, now.day);

    DenseTime dt = TimeDateToDense(now);
    printf("Dense=%llu\n", dt);

    /*LibClose(lib);*/
    ArenaFree(&arena);
    return 0;
}
