
#include "term.h"
#include "unistd.h"

int TermGetWindowSize(int *rows, int *cols)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 0;
}

void TermDie(const char *s)
{
    // TODO(liam): pull GetLastErrorAsString() func.
    DWORD errID = GetLastErrorAsString();
    fprintf(stderr, "Error '%llu' at '%s'.\n", (memory_index)errID, s);

    ExitProcess(errID);
}

void TermDisableRawMode(struct termios *tm)
{
    fflush(stdout);

    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdin == INVALID_HANDLE_VALUE || hstdin == NULL)
    {
        TermDie("GetStdHandle: stdin");
    }

    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hstdout == INVALID_HANDLE_VALUE || hstdout == NULL)
    {
        TermDie("GetStdHandle: stdout");
    }

    (void)SetConsoleMode(hstdout, tm->OutMode);
    (void)SetConsoleMode(hstdin, tm->InMode);
}

void TermEnableRawMode(struct termios *tm)
{
    // Get handles for stdin and stdout
    // HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    // if (hstdin == INVALID_HANDLE_VALUE)
    // {
    //     TermDie("GetStdHandle: stdin");
    // }

    // HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    // if (hstdout == INVALID_HANDLE_VALUE)
    // {
    //     TermDie("GetStdHandle: stdout");
    // }

    HANDLE hstdin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hstdin == INVALID_HANDLE_VALUE)
    {
        TermDie("CreateFile: stdin");
    }

    HANDLE hstdout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hstdin == INVALID_HANDLE_VALUE)
    {
        TermDie("CreateFile: stdout");
    }

    // Set console to "raw" mode
    // TODO(liam): not working
    if (!GetConsoleMode(hstdin, &tm->InMode))
    {
        TermDie("GetConsoleMode: stdin");
    }
    if (!GetConsoleMode(hstdout, &tm->OutMode))
    {
        TermDie("GetConsoleMode: stdout");
    }

    DWORD dwInMode = tm->InMode;
    dwInMode &= ~ENABLE_ECHO_INPUT;
    dwInMode &= ~ENABLE_LINE_INPUT;
    dwInMode &= ~ENABLE_PROCESSED_INPUT;
    dwInMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    DWORD dwOutMode = tm->OutMode;
    dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwOutMode |= ENABLE_PROCESSED_OUTPUT;
    dwOutMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;

    if (!SetConsoleMode(hstdin, dwInMode))
    {
        TermDie("SetConsoleMode: stdin");
    }
    if (!SetConsoleMode(hstdout, dwOutMode))
    {
        TermDie("SetConsoleMode: stdout");
    }
}