#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "base/arena.h"
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

int getCursorPosition(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
        {
            return getCursorPosition(rows, cols);
        }
        /*editorReadKey();*/
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void TermRenderAppend(Arena *arena, TermRenderBuf *rb, uint8 *s, uint32 len)
{
    if (rb->length + len > rb->capacity)
    {
        uint32 newCap = Max(rb->length + len, rb->capacity * 2);
        char *new = PushArray(arena, char, newCap);
        for (uint32 i = 0; i < rb->length; i++)
        {
            *(new + i) = *(rb->b + i);
        }
        rb->b = new;
    }

    for (uint32 i = 0; i < len; i++)
    {
        *(rb->b + rb->length + i) = *(s + i);
    }
    rb->length += len;
}

void TermRender(Arena *arena)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);
    TermRenderBuf = {0};

    // NOTE(liam): hides the cursor during drawing.
    abAppend(arena, &ab, "\x1b[?25l", 6);


    // NOTE(liam): unhides cursor.
    abAppend(arena, &ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.length);
    ArenaScratchFree(tmp);
}

void TermDrawMessageBar(Arena *arena, struct abuf *ab, StringData msg)
{
    abAppend(arena, ab, "\x1b[K", 3);
    memory_index actualLen = msg.size > ;
    if (msglen > E.screenCols) { msglen = E.screenCols; }
    if (msglen && time(NULL) - E.statusMsgTime < 5)
    {
        abAppend(arena, ab, E.statusMsg, msglen);
    }
}
