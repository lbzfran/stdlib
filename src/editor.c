
#include "base/base.h"
#include "os/os.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "gap.h"

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_ARROW_LEFT,
    CTRL_ARROW_RIGHT,
    CTRL_ARROW_UP,
    CTRL_ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,
};

struct editorConfig {
    int cx, cy;
    int rx; // actual x of gap buffer.
    /*int px;*/
    int rowoff;
    int coloff;

    GapBuf buf;
    uint32 *lineStarts;
    uint32 lineCount;

    int screenRows; // total rows available
    int screenCols;
    int numRows;
    /*erow *row;*/

    int dirty;
    int mode;
    StringData filename;
    /*char statusMsg[80];*/
    /*time_t statusMsgTime;*/
    struct termios orig_termios;
    bool32 alive;
};

// TODO(liam): do

static struct editorConfig E;

#define CTRL_KEY(k) ((k) & 0x1f)
int editorReadKey(void);

/*** append buf start ***/

struct abuf {
    char *b;
    uint32 length;
    uint32 capacity;
};

#define ABUF_INIT {NULL, 0}

void abAppend(Arena *arena, struct abuf *ab, char *s, uint32 len)
{
    if (ab->length + len > ab->capacity)
    {
        uint32 newCap = Max(ab->length + len, ab->capacity * 2);
        char *new = PushArray(arena, char, newCap);
        for (uint32 i = 0; i < ab->length; i++)
        {
            *(new + i) = *(ab->b + i);
        }
        ab->b = new;
    }

    /*MemoryCopy(&ab->b[ab->length], s, len);*/
    for (uint32 i = 0; i < len; i++)
    {
        *(ab->b + ab->length + i) = *(s + i);
    }
    ab->length += len;
}

/*** append buf end ***/

void editorDrawRow(struct abuf *ab)
{
}

void editorScroll()
{
    E.rx = 0;
    if (E.cy < E.numRows)
    {
        E.rx = E.cx;
    }

    if (E.cy < E.rowoff)
    {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenRows)
    {
        E.rowoff = E.cy - E.screenRows + 1;
    }
    if (E.rx < E.coloff)
    {
        E.coloff = E.rx;
    }
    if (E.rx >= E.coloff + E.screenCols)
    {
        E.coloff = E.rx - E.screenCols + 1;
    }
}

void editorRefreshScreen(Arena *arena)
{
    editorScroll();
    ArenaTemp tmp = ArenaScratchCreate(arena);
    struct abuf ab = ABUF_INIT;

    write(STDOUT_FILENO, "\x1b[2J", 4);
    /*write(STDOUT_FILENO, "\x1b[H", 3);*/
    abAppend(arena, &ab, "\x1b[?25l", 6);
    /*abAppend(&ab, "\x1b[2J", 4);*/

    GapBuf gb = E.buf;
    int rowCount = 0;

    if (gb.capacity)
    {
        for (int i = 0; i < gb.capacity; i++)
        {
            if (i >= gb.left && i <= gb.right)
            {
                abAppend(arena, &ab, "_", 1);
            }
            else
            {
                char *c = (gb.buf + i);
                if (*c == '\n')
                {
                    /*abAppend(arena, &ab, "\x1b[K", 3);*/
                    /*write(STDOUT_FILENO, "\x1b[K", 3);*/
                    if (rowCount >= E.rowoff && rowCount <= E.screenRows + E.rowoff)
                    {
                        abAppend(arena, &ab, "\r\n", 2);
                    }
                    rowCount++;
                }
                else
                {
                    if (rowCount >= E.rowoff && rowCount <= E.screenRows + E.rowoff)
                    {
                        abAppend(arena, &ab, c, 1);
                    }
                }
            }
        }
    }
    E.numRows = rowCount;
    /*abAppend(arena, &ab, (gb.buf + gb.right), gb.capacity);*/

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                              (E.cx - E.coloff) + 1);
    abAppend(arena, &ab, buf, StringLength((uint8 *)buf));
    abAppend(arena, &ab, "\x1b[?25h", 6);
    /*abAppend(arena, &ab, "\x1b[H", 3);*/

    write(STDOUT_FILENO, ab.b, ab.length);
    ArenaScratchFree(tmp);
}

int getCursorPosition(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    {
        return -1;
    }

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
        {
            break;
        }
        if (buf[i] == 'R')
        {
            break;
        }
        i++;
    }

    if (buf[0] != '\x1b' || buf[1] != '[')
    {
        return -1;
    }
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
    {
        return -1;
    }

    return 0;
}

void editorMoveCursor(int key)
{
    switch (key)
    {
        case ARROW_UP:
        {

        } break;
        case ARROW_DOWN:
        {
            /*while (E.buf.right != 0 && E.buf.buf[E.buf.right] != '\n')*/
        } break;
        case ARROW_LEFT:
        {
            if (GapMoveLeft(&E.buf) && E.cx != 0)
            {
                /*E.cx--;*/
            }
        } break;
        case ARROW_RIGHT:
        {
            if (GapMoveRight(&E.buf) && E.cx != E.screenRows - 2)
            {
                /*E.cx++;*/
            }
        } break;
    }

    int margin = 7;

    if (E.cy < E.rowoff + margin)
    {
        E.rowoff = E.cy - margin;
        if (E.rowoff < 0) { E.rowoff = 0; }
    }
    else if (E.cy >= E.rowoff + E.screenRows - margin)
    {
        E.rowoff = E.cy - E.screenRows + margin + 1;
    }
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
        editorReadKey();
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void editorOpen(Arena *arena, char *filename)
{
    // TODO(liam): fix this part
    E.filename = StringDataNew((uint8 *)filename);

    GapLoad(arena, &E.buf, filename);

    /*editorSelectSyntaxHighlight();*/

    /*char *line = NULL;*/
    /*size_t linecap = 0;*/
    /*ssize_t linelen;*/
    /*while ((linelen = getline(&line, &linecap, fp)) != -1)*/
    /*{*/
    /*    while (linelen > 0 && (line[linelen - 1] == '\n' ||*/
    /*                line[linelen - 1] == '\r'))*/
    /*    { linelen--; }*/
    /*    editorInsertRow(E.numRows, line, linelen);*/
    /*}*/
    /*E.dirty = 0;*/

    /*free(line);*/
}

void editorProcessNewLines()
{

    GapBuf gb = E.buf;
    for (int i = 0; i < gb.left; i++)
    {
        if (gb.buf[i] == '\n')
        {

        }
    }
}

int editorReadKey(void)
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

void editorProcessKeypress(Arena *arena)
{
    static int quitTimes = 2;
    static int resetTimes = 2;
    int c = editorReadKey();

    switch (c)
    {
        case '\r':
        {
            GapInsert(arena, &E.buf, '\r');
            GapInsert(arena, &E.buf, '\n');
            /*E.cy = 0;*/
            /*E.cx = 0;*/
            /*editorInsertNewline();*/
        } break;
        case CTRL_KEY('q'):
        {
            if (E.dirty && quitTimes > 0)
            {
                /*editorSetStatusMessage("UNSAVED CHANGES: "*/
                /*    "Press C-Q %d more times to quit.", quitTimes);*/
                quitTimes--;
                return;
            }
            E.alive = false;
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
            if (E.dirty && resetTimes > 0)
            {
                /*editorSetStatusMessage("UNSAVED CHANGES: "*/
                /*        "Press C-R %d more times to reset file.", resetTimes);*/
                resetTimes--;
                return;
            }
            /*editorReload();*/
        } break;

        case CTRL_KEY('a'):
        case CTRL_KEY('c'):
        case CTRL_KEY('v'):
        case CTRL_KEY('z'):
        {
            /*editorSetStatusMessage("not implemented!");*/
        } break;

        case CTRL_KEY('j'):
        {
            /*editorRowAppendToPrev();*/
        } break;
        case HOME_KEY:
        {
            E.cx = 0;
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
            if (c == DEL_KEY) { editorMoveCursor(ARROW_RIGHT); }
            /*editorDelChar();*/
            GapDelete(&E.buf);
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
            editorMoveCursor(c);
        } break;

        /*case CTRL_KEY('l'):*/
        case '\x1b':
        {
        } break;

        case '\t':
        {
            if (1)
            {
                for (unsigned int i = 0; i < 4; i++)
                {
                    /*editorInsertChar(' ');*/
                    GapInsert(arena, &E.buf, ' ');
                }
                break;
            }
            __attribute__ ((fallthrough));
        }
        default:
        {
            /*GapMove(&E.buf, E.cx);*/
            GapInsert(arena, &E.buf, c);
            /*editorInsertChar(c);*/
        } break;
    }

    quitTimes = 2;
    resetTimes = 2;
}

void editorInit()
{
    /*E.filename = NULL;*/
    E.buf = (GapBuf){ 0 };
    E.alive = true;

    if (getWindowSize(&E.screenRows, &E.screenCols) == -1)
    {
        TermDie("getWindowSize");
    }
    E.screenRows -= 2;
}

int main(int argc, char **argv)
{
    Arena arena = {0};
    TermEnableRawMode(&E.orig_termios);
    editorInit();
    if (argc >= 2)
    {
        editorOpen(&arena, argv[1]);
    }

    while (E.alive)
    {
        editorRefreshScreen(&arena);
        editorProcessKeypress(&arena);
    }

    TermDisableRawMode(&E.orig_termios);
    ArenaFree(&arena);
    return 0;
}
