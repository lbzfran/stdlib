
#include "base/base.h"
#include "term.h"

#include <termios.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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
        TermReadKey();
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void TermSettingsInit(TermSettings *ts)
{
    ts->alive = true;
    ts->statusMsg = (StringData){0};

    if (getWindowSize(&ts->screenRows, &ts->screenCols) == -1)
    {
        TermDie("getWindowSize");
    }
    ts->screenRows -= 2;
}


void TermRenderAppend(Arena *arena, TermRenderBuf *rb, char *s, memory_index len)
{
    if (rb->length + len > rb->capacity)
    {
        uint32 newCap = Max(rb->length + len, rb->capacity * 2);
        uint8 *new = PushArray(arena, uint8, newCap);
        for (memory_index i = 0; i < rb->length; i++)
        {
            *(new + i) = *(rb->b + i);
        }
        rb->b = new;
    }

    for (memory_index i = 0; i < len; i++)
    {
        *(rb->b + rb->length + i) = *(uint8*)(s + i);
    }
    rb->length += len;
}

void TermSetStatusMessage(TermSettings *ts, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char buf[80];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    StringNew(&ts->statusMsg, buf);
    va_end(ap);

    ts->statusMsgTime = time(NULL);
}

void TermDrawMessageBar(Arena *arena, TermSettings *ts, TermRenderBuf *rb)
{
    TermRenderAppend(arena, rb, "\x1b[K", 3);
    memory_index actualLen = ts->statusMsg.size > ts->screenCols ? ts->screenCols : ts->statusMsg.size;
    if (actualLen && time(NULL) - ts->statusMsgTime < 5)
    {
        TermRenderAppend(arena, rb, (char *)StringLiteral(ts->statusMsg), actualLen);
    }
}

void TermRender(Arena *arena, TermSettings *ts)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);
    TermRenderBuf rb = {0};

    // NOTE(liam): hides the cursor during drawing.
    TermRenderAppend(arena, &rb, "\x1b[?25l", 6);
    /*TermRenderAppend(arena, &rb, "\x1b[2J", 4);*/
    /*TermRenderAppend(arena, &rb, "\x1b[H", 3);*/

    /*StringPrintn();*/
    for (memory_index i = 0; i < ts->screenRows - 2; i++)
    {
        TermRenderAppend(arena, &rb, "\r\n", 2);
    }
    TermDrawMessageBar(arena, ts, &rb);

    // NOTE(liam): unhides cursor.
    /*TermRenderAppend(arena, &rb, "\x1b[H", 3);*/
    TermRenderAppend(arena, &rb, "\x1b[?25h", 6);

    write(STDOUT_FILENO, rb.b, rb.length);
    ArenaScratchFree(tmp);
}


int TermReadKey(void)
{
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
        {
            TermDie("read");
        }
    }

    if (c == '\x1b')
    {
        char seq[6] = {0};

        if (read(STDIN_FILENO, &seq[0], 1) != 1) { return '\x1b'; }
        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return '\x1b'; }

        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) { return '\x1b'; }

                if (seq[2] == ';')
                {
                    // TODO(liam): arrow case here; check gpt
                    if (read(STDIN_FILENO, &seq[3], 1) != 1) return '\x1b';
                    // Read the final letter that indicates the arrow direction.
                    if (read(STDIN_FILENO, &seq[4], 1) != 1) return '\x1b';

                    if (seq[3] == '5') { // Modifier 5 means Ctrl.
                        switch (seq[4]) {
                            case 'A': return CTRL_ARROW_UP;
                            case 'B': return CTRL_ARROW_DOWN;
                            case 'C': return CTRL_ARROW_RIGHT;
                            case 'D': return CTRL_ARROW_LEFT;
                        }
                    }
                }
                else if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch(seq[1])
            {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }

        return '\x1b';
    }
    else
    {
        return c;
    }
}

void TermProcessKeypress(TermSettings *ts)
{
    /*static int quitTimes = WEISS_QUIT_CONFIRM_COUNTER;*/
    /*static int resetTimes = WEISS_QUIT_CONFIRM_COUNTER;*/
    int c = TermReadKey();

    switch (c)
    {
        case '\r':
        {
            /*editorInsertNewline();*/
        } break;
        case CTRL_KEY('q'):
        {
            /*if (E.dirty && quitTimes > 0)*/
            /*{*/
            /*    editorSetStatusMessage("UNSAVED CHANGES: "*/
            /*        "Press C-Q %d more times to quit.", quitTimes);*/
            /*    quitTimes--;*/
            /*    return;*/
            /*}*/
            /*write(STDOUT_FILENO, "\x1b[2J", 4);*/
            /*write(STDOUT_FILENO, "\x1b[H", 3);*/
            /*write(STDIN_FILENO, "\033[2J\033[H\033[?1049l", 15);*/
            ts->alive = false;
        } break;

        case CTRL_KEY('s'):
        {
            /*editorSave();*/
        } break;

        case CTRL_KEY('l'):
        {
            /*E.rowoff = getScreenCenter();*/
        } break;

        case CTRL_KEY('r'):
        {
            // NOTE(liam): reopens current file without saving.
            /*if (E.dirty && resetTimes > 0)*/
            /*{*/
            /*    editorSetStatusMessage("UNSAVED CHANGES: "*/
            /*            "Press C-R %d more times to reset file.", resetTimes);*/
            /*    resetTimes--;*/
            /*    return;*/
            /*}*/
            /*editorReload();*/
        } break;

        case CTRL_KEY('a'):
        case CTRL_KEY('c'):
        case CTRL_KEY('v'):
        case CTRL_KEY('z'):
        {
            TermSetStatusMessage(ts, "not implemented!");
        } break;

        case CTRL_KEY('j'):
        {
            /*editorRowAppendToPrev();*/
        } break;
        case HOME_KEY:
        {
            /*E.cx = 0;*/
        } break;
        case END_KEY:
        {
            /*if (E.cy < E.numRows)*/
            /*{*/
            /*    E.cx = E.row[E.cy].size;*/
            /*}*/
        } break;

        case CTRL_KEY('n'):
        {
            /*editorIndentDown();*/
        } break;
        case CTRL_KEY('p'):
        {
            /*editorIndentUp();*/
        } break;

        case CTRL_KEY('f'):
        {
            /*editorFind();*/
        } break;

        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
        {
            /*if (c == DEL_KEY) { editorMoveCursor(ARROW_RIGHT); }*/
            /*editorDelChar();*/
        } break;

        case PAGE_UP:
        case PAGE_DOWN:
        {
            /*if (c == PAGE_UP)*/
            /*{*/
            /*    E.cy = E.rowoff;*/
            /*}*/
            /*else if (c == PAGE_DOWN)*/
            /*{*/
            /*    E.cy = E.rowoff + E.screenRows - 1;*/
            /*    if (E.cy > E.numRows) { E.cy = E.numRows; }*/
            /*}*/
            /**/
            /*int times = E.screenRows;*/
            /*while (times--)*/
            /*{*/
            /*    editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);*/
            /*}*/
        } break;

        case CTRL_ARROW_UP:
        case CTRL_ARROW_DOWN:
        case CTRL_ARROW_LEFT:
        case CTRL_ARROW_RIGHT:
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        {
            /*editorMoveCursor(c);*/
        } break;

        /*case CTRL_KEY('l'):*/
        case '\x1b':
        {
        } break;

        case '\t':
        {
            /*if (WEISS_TAB_AS_SPACES)*/
            /*{*/
            /*    for (unsigned int i = 0; i < WEISS_TAB_STOP; i++)*/
            /*    {*/
            /*        editorInsertChar(' ');*/
            /*    }*/
            /*    break;*/
            /*}*/
            /*__attribute__ ((fallthrough));*/
        }
        default:
        {
            /*editorInsertChar(c);*/
        } break;
    }

    /*quitTimes = WEISS_QUIT_CONFIRM_COUNTER;*/
    /*resetTimes = WEISS_QUIT_CONFIRM_COUNTER;*/
}
