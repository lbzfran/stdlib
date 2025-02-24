
#include "shell.h"

#include <unistd.h>
#include <sys/wait.h>

char *ShellReadLine(Arena *arena, char *prompt)
{
    memory_index bufSize = SHELL_LINE_BUFSIZE;
    char *buf = PushArray(arena, char, bufSize);

    fputs(prompt, stdout);

    char c;
    uint32 pos = 0;
    while (true)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            buf[pos] = '\0';
            return buf;
        }
        else
        {
            buf[pos] = c;
        }
        pos++;

        if (pos >= bufSize)
        {
            bufSize += SHELL_LINE_BUFSIZE;
            buf = PushCopy(arena, bufSize, buf);
        }
    }
}

char **ShellParseLine(char *line)
{
    Assert(false && "Not implemented yet nerd.");

    return NULL;
}

StringList ShellParseStringList(Arena *arena, char *line)
{
    StringList res = StringSplit(arena, StringDataNew(line), SHELL_DELIM);

    return res;
}

void ShellExecLine(char **args)
{
    pid_t pid;
    pid_t __attribute__ ((unused)) wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // NOTE(liam): child
        if (execvp(args[0], args) == -1)
        {
            perror("execvp");
        }
        Throw("failed to execute cmd");
    }
    else if (pid < 0)
    {
        perror("fork");
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void ShellExecStringList(Arena *arena, StringList arglist)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);

    // NOTE(liam): process arglist to char array.
    char **buf = StringListToArray(arena, arglist);

    ShellExecLine(buf);

    ArenaScratchFree(tmp);
}

