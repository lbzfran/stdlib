
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
    Assert(false && "not implemented");
}

void TermDisableRawMode(struct termios *tm) {
    fflush(stdout);

    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);


    (void)SetConsoleMode(tm->stdout, tm->origOutMode);
    (void)SetConsoleMode(tm->stdin, tm->origInMode);
}

void TermEnableRawMode(struct termios *tm) {

    // Get handles for stdin and stdout
    tm->stdin = GetStdHandle(STD_INPUT_HANDLE);
    if (tm->stdin == INVALID_HANDLE_VALUE || tm->stdin == NULL)
    {
        TermDie("GetStdHandle: stdin");
    }

    tm->stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (tm->tdout == INVALID_HANDLE_VALUE || tm->stdout == NULL)
    {
        TermDie("GetStdHandle: stdout");
    }

    // Set console to "raw" mode

    if (!GetConsoleMode(tm->stdin, &tm->origInMode))
    {
        TermDie("GetConsoleMode: stdin");
    }
    if (!GetConsoleMode(tm->stdout, &tm->origOutMode))
    {
        TermDie("GetConsoleMode: stdout");
    }

    DWORD dwInMode = tm->origInMode;
    dwInMode &= ~ENABLE_ECHO_INPUT;
    dwInMode &= ~ENABLE_LINE_INPUT;
    dwInMode &= ~ENABLE_PROCESSED_INPUT;
    dwInMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    DWORD dwOutMode = tm->origOutMode;
    dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwOutMode |= ENABLE_PROCESSED_OUTPUT;
    dwOutMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;

    if (!SetConsoleMode(tm->stdin, dwInMode))
    {
        TermDie("SetConsoleMode: stdin");
    }
    if (!SetConsoleMode(tm->stdout, dwOutMode))
    {
        TermDie("SetConsoleMode: stdout");
    }

    return 0;
}