
#include "shell.h"

#include <unistd.h>
#include <sys/wait.h>

struct command {
  const char **argv;
};

int spawn_proc(int in, int out, struct command *cmd)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        if (in != 0)
        {
            dup2 (in, 0);
            close (in);
        }

        if (out != 1)
        {
            dup2 (out, 1);
            close (out);
        }

        return execvp(cmd->argv [0], (char * const *)cmd->argv);
    }

    return pid;
}

int fork_pipes(int n, struct command *cmd)
{
    int i;
    pid_t pid;
    int in, fd [2];

    /* The first process should get its input from the original file descriptor 0.  */
    in = 0;

    /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
    for (i = 0; i < n - 1; ++i)
    {
        pipe (fd);

        /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
        spawn_proc(in, fd [1], cmd + i);

        /* No need for the write end of the pipe, the child will write here.  */
        close(fd [1]);

        /* Keep the read end of the pipe, the next child will read from there.  */
        in = fd[0];
    }

    /* Last stage of the pipeline - set stdin be the read end of the previous pipe
       and output to the original file descriptor 1. */
    if (in != 0)
    {
        dup2(in, 0);
    }

    /* Execute the last stage with the current process. */
    return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}

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
    char buf[1024];
    ssize_t bufRead;

    int pipefd[2] = {0, 0};

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        Throw("failed to create pipe");
    }

    pid = fork();
    if (pid == 0)
    {
        // NOTE(liam): child

        // NOTE(liam): close read pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
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
        /*do*/
        /*{*/

        // NOTE(liam): close write pipe
        close(pipefd[1]);

        while ((bufRead = read(pipefd[0], buf, sizeof(buf) - 1)) > 0)
        {
            buf[bufRead] = '\0';
            printf("%s", buf);
        }

        close(pipefd[0]);
        wpid = waitpid(pid, &status, 0);
        Assert(wpid == pid);
        /*}*/
        /*while (!WIFEXITED(status) && !WIFSIGNALED(status));*/
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

