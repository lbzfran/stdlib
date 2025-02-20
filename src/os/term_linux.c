#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "term.h"

void TermDie(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    write(STDIN_FILENO, "\033[2J\033[H\033[?1049l", 15);

    perror(s);
    exit(1);
}

void TermEnableRawMode(struct termios *tm)
{
    /*
     * Turns off several terminal bindings (C-c, C-v, C-d, C-m, etc.),
     * turns off terminal echo, and other misc settings.
     */
    if (tcgetattr(STDIN_FILENO, tm) == -1)
    {
        TermDie("tcsetattr");
    }

    struct termios raw = *tm;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        TermDie("tcsetattr");
    }

    // NOTE(liam): turns on alternate screen buffer.
    write(STDIN_FILENO, "\033[?1049h\033[2J\033[H", 15);
}

void TermDisableRawMode(struct termios *tm)
{
    // NOTE(liam): be nice and call this function after.
    write(STDIN_FILENO, "\033[2J\033[H\033[?1049l", 15);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, tm) == -1)
    {
        TermDie("tcsetattr");
    }
}

