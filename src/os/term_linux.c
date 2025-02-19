#include <termios.h>
#include <stdio.h>

void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    write(STDIN_FILENO, "\033[2J\033[H\033[?1049l", 15);

    perror(s);
    exit(1);
}

void enableRawMode(struct termios tm)
{
    /*
     * Turns off several terminal bindings (C-c, C-v, C-d, C-m, etc.),
     * turns off terminal echo, and other misc settings.
     */
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
    {
        die("tcsetattr");
    }
    /*atexit(disableRawMode);*/

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        die("tcsetattr");
    }

    // NOTE(liam): turns on alternate screen buffer.
    write(STDIN_FILENO, "\033[?1049h\033[2J\033[H", 15);
}

void disableRawMode(void)
{
    write(STDIN_FILENO, "\033[2J\033[H\033[?1049l", 15);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    {
        die("tcsetattr");
    }
}
